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
my_IncrementalValueModel value_1 = my_IncrementalValueModel("val1", 0, 10, false, 1);
my_IncrementalValueModel value_2 = my_IncrementalValueModel("val2", -20, 3, false, 1);

/// 3- create 3 serial terminal widget associated with incremental value objects.
my_IncrementalValueWidgetOnSerialMonitor value_0_widget = my_IncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_0);
my_IncrementalValueWidgetOnSerialMonitor value_1_widget = my_IncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_1);
my_IncrementalValueWidgetOnSerialMonitor value_2_widget = my_IncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_2);

QueueHandle_t text_buffer_queue = xQueueCreate(3, sizeof(char *));
SemaphoreHandle_t data_sent = xSemaphoreCreateBinary(); // synchro between display task and sending task

static QueueHandle_t encoder_clk_isr_queue = xQueueCreate(5, sizeof(struct_SwitchButtonIRQData));
static QueueHandle_t central_switch_isr_queue = xQueueCreate(5, sizeof(struct_SwitchButtonIRQData));

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
//---------------------------------------------
my_TestManager manager = my_TestManager();
/// 5- create a widget for the manager
my_ManagerWidget manager_widget = my_ManagerWidget(&my_serial_monitor, &manager);
//-----------------

struct_rtosConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1500,
    .time_out_delay_ms = 5000};
rtos_SwitchButton central_switch = rtos_SwitchButton(CENTRAL_SWITCH_GPIO,
                                                     &ky040_encoder_irq_call_back, central_switch_isr_queue, manager.control_event_input_queue,
                                                     cfg_central_switch);
//-----------------
struct_rtosConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1000,
    .time_out_delay_ms = 3000};
rtos_RotaryEncoder encoder = rtos_RotaryEncoder(ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                                &ky040_encoder_irq_call_back, encoder_clk_isr_queue, manager.control_event_input_queue,
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

void manager_process_control_event_task(void *)
{
    manager.add_managed_model(&value_0);
    manager.add_managed_model(&value_1);
    manager.add_managed_model(&value_2);
    manager.make_manager_active();
    manager.draw_refresh_all_attached_widgets();
    struct_ControlEventData local_event_data;

    while (true)
    {
        xQueueReceive(manager.control_event_input_queue, &local_event_data, portMAX_DELAY);
        p1.hi();
        manager.process_control_event(local_event_data.event);

        p1.lo();
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

void manager_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        manager_widget.send_image_to_DisplayGateKeeper(text_buffer_queue, data_sent);
    }
}
// void value_widget_task_0(void *probe)
// {
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//         value_0_widget.send_text_to_DisplayGateKeeper(text_buffer_queue, data_sent);
//     }
// }
// void value_widget_task_1(void *probe)
// {
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//         value_1_widget.send_text_to_DisplayGateKeeper(text_buffer_queue, data_sent);
//     }
// }
// void value_widget_task_2(void *probe)
// {
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//         value_2_widget.send_text_to_DisplayGateKeeper(text_buffer_queue, data_sent);
//     }
// }

// void display_gate_keeper_task(void *probe)
// {
//     char * text_to_tprint;

//     while (true)
//     {
//         xQueueReceive(text_buffer_queue, &text_to_tprint, portMAX_DELAY);
//         ((Probe *)probe)->hi();
//         my_serial_monitor.show_from_display_queue(text_to_tprint);
//         ((Probe *)probe)->lo();
//         xSemaphoreGive(data_sent);
//     }
// }

int main()
{
    stdio_init_all();

    xTaskCreate(idle_task, "idle_task", 256, &p0, 0, NULL);

    // xTaskCreate(my_model_task, "main_task", 256, &p1, 4, NULL);
    // xTaskCreate(value_widget_task_0, "widget_task_0", 256, NULL, 2, &value_0_widget.task_handle);
    // xTaskCreate(value_widget_task_1, "widget_task_1", 256, NULL, 2, &my_rtos_widget2.task_handle);
    // xTaskCreate(value_widget_task_2, "widget_task_2", 256, NULL, 2, &my_rtos_widget2.task_handle);

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
