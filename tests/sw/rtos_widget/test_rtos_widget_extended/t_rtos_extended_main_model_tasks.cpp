#include "t_rtos_extended_main_model_tasks.h"

#include "utilities/probe/probe.h"

struct_ConfigTextWidget focus_indicator_config = {
    .number_of_column = 10,
    .number_of_line = 1,
    .widget_anchor_x = 4,
    .widget_anchor_y = 140,
    .font = font_12x16,
    .widget_with_border = true};

void idle_task(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void position_controller_task(void *probe)
{
    position_controller.add_managed_rtos_model(&my_rtos_model.angle);
    position_controller.add_managed_rtos_model(&my_rtos_model.y_pos);
    position_controller.add_managed_rtos_model(&my_rtos_model.x_pos);
    position_controller.notify_all_linked_widget_task();

    struct_ControlEventData local_event_data;
    BaseType_t global_timeout_condtion;
    while (true)
    {
        if (probe != NULL)
            ((Probe *)probe)->pulse_us(10);
        position_controller.process_event_and_time_out_condition(&position_controller, UI_MANAGER_TIMEOUT_DELAY_ms);
    }
};

void my_model_task(void *probe)
{
    my_rtos_model.update_attached_rtos_widget(&SSD1306_graph_widget);
    my_rtos_model.update_attached_rtos_widget(&SSD1306_values_widget);
    my_rtos_model.update_attached_rtos_widget(&ST7735_graph_widget);
    my_rtos_model.update_attached_rtos_widget(&ST7735_values_widget);
    my_rtos_model.notify_all_linked_widget_task();

    while (true)
    {
        struct_ControlEventData data;
        while (true)
        {
            xQueueReceive(my_rtos_model.control_event_input_queue, &data, portMAX_DELAY);
            if (probe != NULL)
                ((Probe *)probe)->hi();
            my_rtos_model.process_control_event(data);
            if (probe != NULL)
                ((Probe *)probe)->lo();
        }
    }
}

void angle_evolution_task(void *probe) // periodic task
{
    struct_ControlEventData data;
    data.gpio_number = DUMMY_GPIO_FOR_PERIODIC_EVOLUTION;
    data.event = UIControlEvent::INCREMENT;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true)
    {
        if (probe != NULL)
            ((Probe *)probe)->pulse_us();
        xQueueSend(my_rtos_model.control_event_input_queue, &data, portMAX_DELAY);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(REFRESH_PERIOD_ms));
    }
}

void controlled_position_task(void *position)
{
    struct_ControlEventData received_control_event;
    rtos_UIControlledModel *center_position = (rtos_UIControlledModel *)position;
    while (true)
    {
        xQueueReceive(center_position->control_event_input_queue, &received_control_event, portMAX_DELAY);
        center_position->process_control_event(received_control_event);
    }
}


void SPI_focus_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        SPI_focus_indicator_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&SPI_focus_indicator_widget);
    }
}

//------------------------- ST7735 value widget---------------------
void SPI_values_widget_task(void *probe)
{
    my_text_widget ST7735_title_widget = my_text_widget(&color_display, ST7735_title_config,
                                                        ST7735_TEXT_CANVAS_FORMAT);
    ST7735_title_widget.writer->write("ANGLEH_POSV_POS");
    SPI_display_gate_keeper.send_widget_data(&ST7735_title_widget);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        ST7735_values_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&ST7735_values_widget);
    }
}
//------------------------- ST7735 graph widget---------------------
void SPI_graph_widget_task(void *probe)
{
    SPI_display_gate_keeper.send_clear_device_command(&color_display);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        ST7735_graph_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&ST7735_graph_widget);
    }
}

//-------------SSD1306  value widgets-----
void I2C_left_values_widget_task(void *probe)
{
    I2C_display_gate_keeper.send_clear_device_command(&left_display);
    my_text_widget left_title = my_text_widget(&left_display, SSD1306_title_config,
                                               SSD1306_CANVAS_FORMAT);
    left_title.writer->write("ANGLEH_POSV_POS");
    I2C_display_gate_keeper.send_widget_data(&left_title);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        SSD1306_values_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        I2C_display_gate_keeper.send_widget_data(&SSD1306_values_widget);
    }
}
//----------------------SSD1306 graph widget---------------------------
void I2C_right_graph_widget_task(void *probe)
{
    I2C_display_gate_keeper.send_clear_device_command(&right_display);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        SSD1306_graph_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        I2C_display_gate_keeper.send_widget_data(&SSD1306_graph_widget);
    }
}
