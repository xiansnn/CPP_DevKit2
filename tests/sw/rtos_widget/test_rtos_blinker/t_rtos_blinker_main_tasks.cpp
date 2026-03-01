#include "t_rtos_blinker_main_tasks.h"
#include "utilities/probe/probe.h"

void idle_task(void *probe)
{
    while (true)
    {
        if (probe != NULL)
        {
            ((Probe *)probe)->hi();
            ((Probe *)probe)->lo();
        }
    }
}

void my_clock_controller_task(void *probe)
{
    my_clock_controller.add_managed_rtos_model(my_clock.hour);
    my_clock_controller.add_managed_rtos_model(my_clock.minute);
    my_clock_controller.add_managed_rtos_model(my_clock.second);
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

void my_clock_main_task(void *probe)
{
    my_clock.notify_all_linked_widget_task();
    my_clock.hour->notify_all_linked_widget_task();
    my_clock.minute->notify_all_linked_widget_task();
    my_clock.second->notify_all_linked_widget_task();

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

void my_clock_controlled_hour_task(void *probe)
{
    while (true)
    {
        struct_ControlEventData data;
        while (true)
        {
            xQueueReceive(my_clock.hour->control_event_input_queue, &data, portMAX_DELAY);
            if (probe != NULL)
                ((Probe *)probe)->hi();
            my_clock.hour->process_control_event(data);
            if (probe != NULL)
                ((Probe *)probe)->lo();
        }
    }
}

void my_clock_controlled_minute_task(void *probe)
{
    while (true)
    {
        struct_ControlEventData data;
        while (true)
        {
            xQueueReceive(my_clock.minute->control_event_input_queue, &data, portMAX_DELAY);
            if (probe != NULL)
                ((Probe *)probe)->hi();
            my_clock.minute->process_control_event(data);
            if (probe != NULL)
                ((Probe *)probe)->lo();
        }
    }
}

void my_clock_controlled_second_task(void *probe)
{
    while (true)
    {
        struct_ControlEventData data;
        while (true)
        {
            xQueueReceive(my_clock.second->control_event_input_queue, &data, portMAX_DELAY);
            if (probe != NULL)
                ((Probe *)probe)->hi();
            my_clock.second->process_control_event(data);
            if (probe != NULL)
                ((Probe *)probe)->lo();
        }
    }
}

void my_clock_timer_task(void *probe) // periodic task
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
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CLOCK_REFRESH_PERIOD_ms));
    }
}

void SPI_analog_clock_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        analog_clock_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&analog_clock_widget);
    }
}

void blinker_task(void *probe)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (true)
    {
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_blinker.refresh_blinking();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(my_blinker.blink_period_ms));
    }
}

void SPI_digital_clock_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        digital_clock_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(digital_clock_widget.clock_hour_widget_element);
        SPI_display_gate_keeper.send_widget_data(digital_clock_widget.clock_minute_widget_element);
        SPI_display_gate_keeper.send_widget_data(digital_clock_widget.clock_second_widget_element);
    }
}

void SPI_hour_digital_widget_task(void *probe)
{
    SPI_display_gate_keeper.send_clear_device_command(&color_display);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        digital_clock_widget.clock_hour_widget_element->draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(digital_clock_widget.clock_hour_widget_element);
    }
}

void SPI_minute_digital_widget_task(void *probe)
{

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        digital_clock_widget.clock_minute_widget_element->draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(digital_clock_widget.clock_minute_widget_element);
    }
}

void SPI_second_digital_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        digital_clock_widget.clock_second_widget_element->draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(digital_clock_widget.clock_second_widget_element);
    }
}

void SPI_hour_analog_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        analog_clock_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&analog_clock_widget);
    }
}

void SPI_minute_analog_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        analog_clock_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&analog_clock_widget);
    }
}

void SPI_second_analog_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        analog_clock_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&analog_clock_widget);
    }
}
