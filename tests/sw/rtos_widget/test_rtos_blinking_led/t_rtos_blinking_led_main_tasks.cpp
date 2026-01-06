#include "t_rtos_blinking_led_main_tasks.h"
#include "utilities/probe/probe.h"

void idle_task(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void focus_led_manager_task(void *probe)
{
    focus_led_manager.add_managed_rtos_model(&my_clock.hour);
    focus_led_manager.add_managed_rtos_model(&my_clock.minute);
    focus_led_manager.add_managed_rtos_model(&my_clock.second);
    focus_led_manager.notify_all_linked_widget_task();

    struct_ControlEventData local_event_data;
    BaseType_t global_timeout_condtion;
    while (true)
    {
        if (probe != NULL)
            ((Probe *)probe)->pulse_us(10);
        focus_led_manager.process_event_and_time_out_condition(&focus_led_manager, TIMEOUT_UI_MANAGER_DELAY_ms);
    }
};

void my_main_clock_task(void *probe)
{
    // my_rtos_model.update_attached_rtos_widget(&SSD1306_graph_widget);
    // my_rtos_model.update_attached_rtos_widget(&SSD1306_values_widget);
    // my_rtos_model.update_attached_rtos_widget(&ST7735_graph_widget);
    // my_rtos_model.update_attached_rtos_widget(&ST7735_values_widget);
    // my_clock.hour.update_attached_rtos_widget(&my_main_clock_dummy_widget);
    my_clock.notify_all_linked_widget_task();

    while (true)
    {
        struct_ControlEventData data;
        while (true)
        {
            xQueueReceive(my_clock.control_event_input_queue, &data, portMAX_DELAY);
            if (probe != NULL)
                ((Probe *)probe)->hi();
            my_clock.process_control_event(data);
            if (probe != NULL)
                ((Probe *)probe)->lo();
        }
    }
}

void my_main_clock_hour_task(void *probe)
{
    while (true)
    {
        struct_ControlEventData data;
        while (true)
        {
            xQueueReceive(my_clock.hour.control_event_input_queue, &data, portMAX_DELAY);
            if (probe != NULL)
                ((Probe *)probe)->hi();
            my_clock.hour.process_control_event(data);
            if (probe != NULL)
                ((Probe *)probe)->lo();
        }
    }
}

void my_main_clock_minute_task(void *probe)
{
    while (true)
    {
        struct_ControlEventData data;
        while (true)
        {
            xQueueReceive(my_clock.minute.control_event_input_queue, &data, portMAX_DELAY);
            if (probe != NULL)
                ((Probe *)probe)->hi();
            my_clock.minute.process_control_event(data);
            if (probe != NULL)
                ((Probe *)probe)->lo();
        }
    }
}

void my_main_clock_second_task(void *probe)
{
    while (true)
    {
        struct_ControlEventData data;
        while (true)
        {
            xQueueReceive(my_clock.second.control_event_input_queue, &data, portMAX_DELAY);
            if (probe != NULL)
                ((Probe *)probe)->hi();
            my_clock.second.process_control_event(data);
            if (probe != NULL)
                ((Probe *)probe)->lo();
        }
    }
}

void one_second_timer_task(void *probe) // periodic task
{
    struct_ControlEventData data;
    data.gpio_number = GPIO_dummy_FOR_PERIODIC_EVOLUTION;
    data.event = UIControlEvent::INCREMENT;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true)
    {
        if (probe != NULL)
            ((Probe *)probe)->pulse_us();
        xQueueSend(my_clock.control_event_input_queue, &data, portMAX_DELAY);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(PERIOD_REFRESH_ms));
    }
}

void focus_led_manager_dummy_widget_task(void *probe)
{    
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_focus_manager_dummy_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
    }
}

void main_clock_dummy_widget_task(void *probe)
{    
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_main_clock_dummy_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
    }
}
