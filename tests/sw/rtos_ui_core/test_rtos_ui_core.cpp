#include "sw/ui_core/ui_core.h"
#include "sw/widget/widget.h"
#include "sw/display_device/display_device.h"

#include "sw/ui_core/rtos_ui_core.h"
#include "device/rotary_encoder/rtos_rotary_encoder.h"

#include <map>
#include <string>

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

#define GLOBAL_TIMEOUT_DELAY_ms 5000
#define SW_TIMEOUT_DELAY_ms 1000
#define UI_MANAGER_TIMEOUT_DELAY_ms GLOBAL_TIMEOUT_DELAY_ms - SW_TIMEOUT_DELAY_ms
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

//---------------Printer Device-------------
TerminalConsole my_serial_monitor = TerminalConsole(100, 1);

//---------------incremental value object--------------
my_IncrementalValueModel value_0 = my_IncrementalValueModel("val0", 0, 5, true, 1);
my_IncrementalValueModel value_1 = my_IncrementalValueModel("val1", 0, 10, false, 1);
my_IncrementalValueModel value_2 = my_IncrementalValueModel("val2", -20, 3, false, 1);

//---------------serial terminal widget associated with incremental value objects---------------------------
my_IncrementalValueWidgetOnSerialMonitor value_0_widget = my_IncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_0);
my_IncrementalValueWidgetOnSerialMonitor value_1_widget = my_IncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_1);
my_IncrementalValueWidgetOnSerialMonitor value_2_widget = my_IncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_2);

QueueHandle_t text_buffer_queue = xQueueCreate(3, sizeof(char *));
SemaphoreHandle_t data_sent = xSemaphoreCreateBinary(); // synchro between display task and sending task

void ky040_encoder_irq_call_back(uint gpio, uint32_t event_mask);

//---------------my_TestManager manager------------------------------
my_TestManager manager = my_TestManager(true);
my_ManagerWidgetOnSerialMonitor manager_widget = my_ManagerWidgetOnSerialMonitor(&my_serial_monitor, &manager);

//-----KY040---------
//                   rtos_SwitchButton central_switch---------------------
struct_rtosConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1500,
    .time_out_delay_ms = SW_TIMEOUT_DELAY_ms};
rtos_SwitchButton central_switch = rtos_SwitchButton(CENTRAL_SWITCH_GPIO,
                                                     &ky040_encoder_irq_call_back, manager.control_event_input_queue,
                                                     cfg_central_switch);
void central_switch_process_irq_event_task(void *)
{
    central_switch.rtos_process_IRQ_event();
}
//                   rtos_RotaryEncoder encoder-------------------------------
struct_rtosConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1000,
    .time_out_delay_ms = SW_TIMEOUT_DELAY_ms};
rtos_RotaryEncoder encoder = rtos_RotaryEncoder(ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                                &ky040_encoder_irq_call_back, manager.control_event_input_queue,
                                                cfg_encoder_clk);
void encoder_process_irq_event_task(void *)
{
    encoder.rtos_process_IRQ_event();
}
//                 ky040_encoder_irq_call_back--------------------------------
void ky040_encoder_irq_call_back(uint gpio, uint32_t event_mask)
{
    struct_SwitchButtonIRQData data;
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    data.current_time_us = time_us_32();
    data.event_mask = event_mask;
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    switch (gpio)
    {
    case CENTRAL_SWITCH_GPIO:
        xQueueSendFromISR(central_switch.IRQdata_input_queue, &data, &pxHigherPriorityTaskWoken);
        break;
    case ENCODER_CLK_GPIO:
        xQueueSendFromISR(encoder.IRQdata_input_queue, &data, &pxHigherPriorityTaskWoken);
        break;
    default:
        break;
    }
    portYIELD_FROM_ISR(&pxHigherPriorityTaskWoken);
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
};
//------------------------ TASKS---------------------------------------
void idle_task(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void UI_control_event_manager_task(void *)
{
    manager.add_managed_rtos_model(&value_0);
    manager.add_managed_rtos_model(&value_1);
    manager.add_managed_rtos_model(&value_2);

    p1.hi();
    manager.notify_all_linked_widget_task();
    p1.lo();

    struct_ControlEventData local_event_data;
    BaseType_t global_timeout_condtion;
    while (true)
    {
        global_timeout_condtion = xQueueReceive(manager.control_event_input_queue,
                                                &local_event_data,
                                                ((manager.get_rtos_status() == ControlledObjectStatus::IS_IDLE) ? portMAX_DELAY : pdMS_TO_TICKS(UI_MANAGER_TIMEOUT_DELAY_ms)));
        p1.hi();
        if (global_timeout_condtion == errQUEUE_EMPTY)
        {
            local_event_data.event = UIControlEvent::TIME_OUT;
            manager.process_control_event(local_event_data);
        }
        else if (local_event_data.event != UIControlEvent::TIME_OUT) // switch and encoder timout is replaced by a global timeout
        {
            manager.process_control_event(local_event_data);
        }
        p1.lo();
    }
};

void all_incremental_value_task(void *value_x)
{
    struct_ControlEventData received_control_event;
    my_IncrementalValueModel *incremental_value_model = (my_IncrementalValueModel *)value_x;
    while (true)
    {
        xQueueReceive(incremental_value_model->control_event_input_queue, &received_control_event, portMAX_DELAY);
        p2.hi();
        incremental_value_model->process_control_event(received_control_event);
        p2.lo();
    }
}

void manager_widget_task(void *)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        p3.hi();
        manager_widget.draw();
        ((TerminalConsole*)manager_widget.display_device)->show();
        // manager_widget.send_text_to_DisplayGateKeeper(text_buffer_queue, data_sent);
        p3.lo();
    }
}

void all_incremental_value_widget_task(void *value_widget)
{
    my_IncrementalValueWidgetOnSerialMonitor *incremental_value_widget = (my_IncrementalValueWidgetOnSerialMonitor *)value_widget;
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        p4.hi();
        incremental_value_widget->draw();
        ((TerminalConsole*)incremental_value_widget->display_device)->show();
        // incremental_value_widget->send_text_to_DisplayGateKeeper(text_buffer_queue, data_sent);
        p4.lo();
    }
}

void display_gate_keeper_task(void *probe)
{
    char *text_to_tprint;

    while (true)
    {
        xQueueReceive(text_buffer_queue, &text_to_tprint, portMAX_DELAY);
        // p7.hi();
        // my_serial_monitor.show_from_display_queue(text_to_tprint);
        // p7.lo();
        xSemaphoreGive(data_sent);
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(idle_task, "idle_task", 256, &p0, 0, NULL);

    xTaskCreate(central_switch_process_irq_event_task, "central_switch_process_irq_event_task", 256, NULL, 4, NULL);
    xTaskCreate(encoder_process_irq_event_task, "encoder_process_irq_event_task", 256, NULL, 4, NULL);

    xTaskCreate(UI_control_event_manager_task, "UI_control_event_manager_task", 256, NULL, 3, &manager.task_handle);

    xTaskCreate(all_incremental_value_task, "value_0_task", 256, &value_0, 3, &value_0.task_handle);
    xTaskCreate(all_incremental_value_task, "value_1_task", 256, &value_1, 3, &value_1.task_handle);
    xTaskCreate(all_incremental_value_task, "value_2_task", 256, &value_2, 3, &value_2.task_handle);

    xTaskCreate(manager_widget_task, "manager_widget_task", 256, NULL, 2, &manager_widget.task_handle);

    xTaskCreate(all_incremental_value_widget_task, "value_0_widget_task", 256, &value_0_widget, 2, &value_0_widget.task_handle);
    xTaskCreate(all_incremental_value_widget_task, "value_1_widget_task", 256, &value_1_widget, 2, &value_1_widget.task_handle);
    xTaskCreate(all_incremental_value_widget_task, "value_2_widget_task", 256, &value_2_widget, 2, &value_2_widget.task_handle);

    // xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 256, NULL, 6, NULL);

    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
