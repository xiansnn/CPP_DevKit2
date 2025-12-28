#include "t_rtos_extended_widgets.h"


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
