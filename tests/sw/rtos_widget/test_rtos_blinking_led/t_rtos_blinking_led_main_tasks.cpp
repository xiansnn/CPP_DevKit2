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

void my_clock_controller_task(void *probe)
{
    my_clock_controller.add_managed_rtos_model(&my_clock.hour);
    my_clock_controller.add_managed_rtos_model(&my_clock.minute);
    my_clock_controller.add_managed_rtos_model(&my_clock.second);
    my_clock_controller.notify_all_linked_widget_task();

    struct_ControlEventData local_event_data;
    BaseType_t global_timeout_condtion;
    while (true)
    {
        if (probe != NULL)
            ((Probe *)probe)->pulse_us(10);
        my_clock_controller.process_event_and_time_out_condition(&my_clock_controller, TIMEOUT_UI_MANAGER_DELAY_ms);
    }
};

void my_main_clock_task(void *probe)
{
    // my_clock.hour.update_attached_rtos_widget(&clock_monitoring_widget);
    // my_clock.minute.update_attached_rtos_widget(&clock_monitoring_widget);
    // my_clock.second.update_attached_rtos_widget(&clock_monitoring_widget);
    
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

void clock_controller_dummy_widget_task(void *probe)
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

void main_clock_hand_widget_task(void *widget)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ((clock_hand_dummy_widget *)widget)->draw();
    }
}

void controller_monitoring_widget_task(void *widget)
{
    my_controller_monitoring_widget *w = (my_controller_monitoring_widget *)widget;

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        w->draw();
        I2C_display_gate_keeper.send_widget_data(w);
    }
}

void clock_monitoring_widget_task(void *widget)
{
    my_controller_monitoring_widget *w = (my_controller_monitoring_widget *)widget;

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        w->draw();
        I2C_display_gate_keeper.send_widget_data(w);
    }
}
