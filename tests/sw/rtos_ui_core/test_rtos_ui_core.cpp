#include "sw/ui_core/ui_core.h"
#include "sw/widget/widget.h"
#include "sw/display_device/display_device.h"

#include "sw/ui_core/rtos_ui_core.h"
#include "device/rotary_encoder/rtos_rotary_encoder.h"

#include <map>
#include <string>

#include "t_rtos_ui_core.h"
#include "t_rtos_controlled_value.h"
#include "t_rtos_manager.h"
#include "t_rtos_widget_on_serial_monitor.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

#define MAIN_TASK_PERIOD_ms 500
#define CENTRAL_SWITCH_GPIO 18
#define ENCODER_CLK_GPIO 19
#define ENCODER_DT_GPIO 20

//----------------------------------
std::map<UIControlEvent, std::string> event_to_string{
    {UIControlEvent::NONE, "NONE"},
    {UIControlEvent::PUSH, "PUSH"},
    {UIControlEvent::DOUBLE_PUSH, "DOUBLE_PUSH"},
    {UIControlEvent::LONG_PUSH, "LONG_PUSH"},
    {UIControlEvent::RELEASED_AFTER_LONG_TIME, "RELEASED_AFTER_LONG_TIME"},
    {UIControlEvent::RELEASED_AFTER_SHORT_TIME, "RELEASED_AFTER_SHORT_TIME"},
    {UIControlEvent::INCREMENT, "INCREMENT"},
    {UIControlEvent::DECREMENT, "DECREMENT"},
    {UIControlEvent::TIME_OUT, "TIME_OUT"}};
//----------------------------
PrinterDevice my_serial_monitor = PrinterDevice(100, 1);
/// 2- create 3 incremental value object
my_IncrementalValueModel value_0 = my_IncrementalValueModel("val0", 0, 5, true, 1);
rtos_my_IncrementalValueModel rtos_value_0 = rtos_my_IncrementalValueModel(&value_0);
// MyIncrementalValueModel value_1 = MyIncrementalValueModel("val1", 0, 10, false, 1);
// MyIncrementalValueModel value_2 = MyIncrementalValueModel("val2", -20, 3, false, 1);

/// 3- create 3 serial terminal widget associated with incremental value objects.
// MyIncrementalValueWidgetOnSerialMonitor value_0_widget = MyIncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_0);
// MyIncrementalValueWidgetOnSerialMonitor value_1_widget = MyIncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_1);
// MyIncrementalValueWidgetOnSerialMonitor value_2_widget = MyIncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_2);

QueueHandle_t display_queue = xQueueCreate(8, sizeof(struct_DataToShow));
SemaphoreHandle_t data_sent = xSemaphoreCreateBinary(); // synchro between display task and sending task

static QueueHandle_t encoder_clk_isr_queue = xQueueCreate(5, sizeof(struct_SwitchButtonIRQData));
static QueueHandle_t central_switch_isr_queue = xQueueCreate(5, sizeof(struct_SwitchButtonIRQData));
static QueueHandle_t control_event_queue_to_manager = xQueueCreate(5, sizeof(struct_ControlEventData));

// //------------------
// my_Model my_model = my_Model();
// rtos_Model my_rtos_model = rtos_Model(&my_model);
// //------------------
// my_ProbePrinter probe_widget1 = my_ProbePrinter(&p2);
// my_Widget my_widget1 = my_Widget(&probe_widget1, &my_model);
// rtos_Widget my_rtos_widget1 = rtos_Widget(&my_widget1);
// //------------------
// my_ProbePrinter probe_widget2 = my_ProbePrinter(&p3);
// my_Widget my_widget2 = my_Widget(&probe_widget2, &my_model);
// rtos_Widget my_rtos_widget2 = rtos_Widget(&my_widget2);

//-----------------
void encoder_clk_irq_call_back(uint gpio, uint32_t event_mask);
void ky040_encoder_irq_call_back(uint gpio, uint32_t event_mask);
//-----------------
struct_rtosConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1500,
    .time_out_delay_ms = 5000};
rtos_SwitchButton central_switch = rtos_SwitchButton(CENTRAL_SWITCH_GPIO,
                                                     &ky040_encoder_irq_call_back, central_switch_isr_queue, control_event_queue_to_manager,
                                                     cfg_central_switch);
//-----------------
struct_rtosConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1000,
    .time_out_delay_ms = 3000};
rtos_RotaryEncoder encoder = rtos_RotaryEncoder(ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                                &ky040_encoder_irq_call_back, encoder_clk_isr_queue, control_event_queue_to_manager,
                                                cfg_encoder_clk);
//-----------------
void ky040_encoder_irq_call_back(uint gpio, uint32_t event_mask)
{
    struct_SwitchButtonIRQData data;
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    data.current_time_us = time_us_32();
    p5.hi();
    data.event_mask = event_mask;
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    switch (gpio)
    {
    case CENTRAL_SWITCH_GPIO:
        p6.hi();
        xQueueSendFromISR(central_switch_isr_queue, &data, &pxHigherPriorityTaskWoken);
        p6.lo();
        break;
    case ENCODER_CLK_GPIO:
        p7.hi();
        xQueueSendFromISR(encoder_clk_isr_queue, &data, &pxHigherPriorityTaskWoken);
        p7.lo();
        break;
    default:
        break;
    }
    portYIELD_FROM_ISR(&pxHigherPriorityTaskWoken);
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    p5.lo();
};
//---------------------------------------------
my_TestManager manager = my_TestManager();
rtos_my_TestManager rtos_manager = rtos_my_TestManager(&manager);
//-----------------
void manager_process_control_event_task(void *)
{
    manager.add_managed_model(&value_0);
    struct_ControlEventData local_event_data;



    while (true)
    {
        xQueueReceive(control_event_queue_to_manager, &local_event_data, portMAX_DELAY);
        rtos_manager.process_control_event_queue(local_event_data);
    }
};
void central_switch_process_irq_event_task(void *)
{
    central_switch.rtos_process_IRQ_event();
}
void encoder_process_irq_event_task(void *)
{
    encoder.rtos_process_IRQ_event();
}


void idle_task(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(idle_task, "idle_task", 256, &p0, 0, NULL);

    // xTaskCreate(my_model_task, "main_task", 256, &p1, 4, NULL);
    // xTaskCreate(widget_task_1, "widget_task_1", 256, NULL, 2, &my_rtos_widget1.task_handle);
    // xTaskCreate(widget_task_2, "widget_task_2", 256, NULL, 2, &my_rtos_widget2.task_handle);

    xTaskCreate(central_switch_process_irq_event_task, "central_switch_process_irq_event_task", 256, NULL, 4, NULL);
    xTaskCreate(encoder_process_irq_event_task, "encoder_process_irq_event_task", 256, NULL, 4, NULL);
    xTaskCreate(manager_process_control_event_task, "manager_process_control_event_task", 256, NULL, 2, NULL);

    // xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 256, &p4, 6, NULL);

    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
// void my_model_task(void *pxProbe)
// {
//     TickType_t xLastWakeTime = xTaskGetTickCount();
//     my_rtos_model.link_widget(&my_rtos_widget1);
//     my_rtos_model.link_widget(&my_rtos_widget2);
//     while (true)
//     {
//         ((my_Model *)my_rtos_model.model)->cyclic_computation((Probe *)pxProbe);
//         my_rtos_model.notify_all_linked_widget_task();
//         vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MAIN_TASK_PERIOD_ms));
//     }
// }
// void widget_task_1(void *probe)
// {
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//         // warning: be sure to complete struct_DataToShow before send_image_to_DisplayGateKeeper
//         my_rtos_widget1.widget->send_image_to_DisplayGateKeeper(display_queue, data_sent);
//     }
// }
// void widget_task_2(void *probe)
// {
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//         my_rtos_widget2.widget->send_image_to_DisplayGateKeeper(display_queue, data_sent);
//     }
// }


// void display_gate_keeper_task(void *probe)
// {
//     struct_DataToShow received_data_to_show;

//     while (true)
//     {
//         xQueueReceive(display_queue, &received_data_to_show, portMAX_DELAY);
//         ((Probe *)probe)->hi();
//         switch (received_data_to_show.command)
//         {
//         case DisplayCommand::SHOW_IMAGE:
//             ((my_ProbePrinter *)received_data_to_show.display)->show_from_display_queue(received_data_to_show);
//             break;
//         case DisplayCommand::CLEAR_SCREEN:
//             ((my_ProbePrinter *)received_data_to_show.display)->clear_device_screen_buffer();
//             break;
//         default:
//             break;
//         };
//         ((Probe *)probe)->lo();
//         xSemaphoreGive(data_sent);
//     }
// }
