/**
 * @file test_rtos_ssd1306_text_and_graph_widgets.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-12-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "t_rtos_common_text_and_graph_widgets.h"
#include "sw/ui_core/rtos_ui_core.h"
#include "device/SSD1306/ssd1306.h"
#include "device/ST7735/st7735.h"
#include "sw/widget/rtos_widget.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
// Probe p2 = Probe(2);
// Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

#define REFRESH_PERIOD_ms 50
#define ROLL_INCREMENT 1

#define SSD1306_CANVAS_FORMAT CanvasFormat::MONO_VLSB
#define ST7735_GRAPHICS_CANVAS_FORMAT CanvasFormat::RGB565_16b
#define ST7735_TEXT_CANVAS_FORMAT CanvasFormat::RGB565_16b

// #define ST7735_128x128
#define ST7735_128x160

#ifdef ST7735_128x128
#define DEVICE_DISPLAY_TYPE ST7735DisplayType::ST7735_144_128_RGB_128_GREENTAB
#define DEVICE_DISPLAY_ROTATION ST7735Rotation::_90
#define DEVICE_DISPLAY_HEIGHT 128
#endif
#ifdef ST7735_128x160
#define DEVICE_DISPLAY_TYPE ST7735DisplayType::ST7735_177_160_RGB_128_GREENTAB
#define DEVICE_DISPLAY_ROTATION ST7735Rotation::_180
#define DEVICE_DISPLAY_HEIGHT 160
#endif

//==========================display gatekeeper===============
rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper = rtos_GraphicDisplayGateKeeper();
rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper = rtos_GraphicDisplayGateKeeper();
//==========================Master I2C========================
void i2c_irq_handler();
struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE,
    .i2c_tx_master_handler = i2c_irq_handler};
rtos_HW_I2C_Master master = rtos_HW_I2C_Master(cfg_i2c);
void i2c_irq_handler()
{
    master.i2c_dma_isr();
};
//==================SSD1306 left screen===================
struct_ConfigSSD1306 cfg_left_screen{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};
rtos_SSD1306 left_display = rtos_SSD1306(&master, cfg_left_screen);
//==================SSD1306 right screen===================
struct_ConfigSSD1306 cfg_right_screen{
    .i2c_address = 0x3D,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};
rtos_SSD1306 right_display = rtos_SSD1306(&master, cfg_right_screen);
//==========================Master SPI========================
struct_ConfigMasterSPI cfg_spi = {
    .spi = spi1,
    .sck_pin = 10,
    .tx_pin = 11,
    .rx_pin = 12,
    .cs_pin = 13,
    .baud_rate_Hz = 10 * 1000 * 1000};
void end_of_TX_DMA_xfer_handler();
rtos_HW_SPI_Master spi_master = rtos_HW_SPI_Master(cfg_spi,
                                                   DMA_IRQ_0, end_of_TX_DMA_xfer_handler);
void end_of_TX_DMA_xfer_handler()
{
    // p7.hi();
    spi_master.spi_tx_dma_isr();
    // p7.lo();
}
//================== ST7735===================
struct_ConfigST7735 cfg_st7735{
    .display_type = DEVICE_DISPLAY_TYPE,
    .backlight_pin = 5,
    .hw_reset_pin = 15,
    .dc_pin = 14,
    .rotation = DEVICE_DISPLAY_ROTATION};
rtos_ST7735 color_display = rtos_ST7735(&spi_master, cfg_st7735);
//-------------Model-----
my_model my_rtos_model = my_model();
//-------------Title-text-----
struct_ConfigTextWidget common_title_config = {
    .number_of_column = 10,
    .number_of_line = 1,
    .widget_anchor_x = 0,
    .widget_anchor_y = 64,
    .font = font_12x16};
my_text_widget left_title = my_text_widget(&left_display, common_title_config, SSD1306_CANVAS_FORMAT);
my_text_widget title = my_text_widget(&color_display, common_title_config, ST7735_TEXT_CANVAS_FORMAT);
//--------------Values-text----
struct_ConfigTextWidget common_values_config = {
    .number_of_column = 10,
    .number_of_line = 1,
    .widget_anchor_x = 0,
    .widget_anchor_y = (uint8_t)(common_title_config.widget_anchor_y + 2 * common_title_config.font[FONT_HEIGHT_INDEX]),
    .font = font_12x16};
my_text_widget my_rtos_left_values_widget = my_text_widget(&left_display, common_values_config, SSD1306_CANVAS_FORMAT, nullptr);
my_text_widget my_rtos_values_widget = my_text_widget(&color_display, common_values_config, ST7735_GRAPHICS_CANVAS_FORMAT, nullptr);
//---------------Graph------------
struct_ConfigGraphicWidget ssd1306_graph_config{
    .canvas_width_pixel = 128,
    .canvas_height_pixel = 56,
    .canvas_foreground_color = ColorIndex::WHITE,
    .canvas_background_color = ColorIndex::BLACK,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};
my_visu_widget my_rtos_right_graph_widget = my_visu_widget(&right_display, ssd1306_graph_config, SSD1306_CANVAS_FORMAT, nullptr);
struct_ConfigGraphicWidget st7735_graph_config{
    .canvas_width_pixel = 128,
    .canvas_height_pixel = 56,
    .canvas_foreground_color = ColorIndex::YELLOW,
    .canvas_background_color = ColorIndex::RED,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};
my_visu_widget my_rtos_graph_widget = my_visu_widget(&color_display, st7735_graph_config, ST7735_GRAPHICS_CANVAS_FORMAT, nullptr);
//---------------TASKS-----------------
void idle_task(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void my_model_task(void *probe)
{
    my_rtos_model.update_attached_rtos_widget(&my_rtos_right_graph_widget);
    my_rtos_model.update_attached_rtos_widget(&my_rtos_left_values_widget);
    my_rtos_model.update_attached_rtos_widget(&my_rtos_graph_widget);
    my_rtos_model.update_attached_rtos_widget(&my_rtos_values_widget);

    int sign = 1;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true)
    {
        sign *= -1;
        for (int i = -90; i < 90; i += ROLL_INCREMENT)
        {
            if (probe != NULL)
                ((Probe *)probe)->hi();
            my_rtos_model.update_cycle(i, sign); //  cyclic_computation((Probe *)pxProbe);
            my_rtos_model.notify_all_linked_widget_task();
            if (probe != NULL)
                ((Probe *)probe)->lo();
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(REFRESH_PERIOD_ms));
        }
    }
}

void SPI_display_gate_keeper_task(void *probe)
{
    struct_WidgetDataToGateKeeper received_data_to_show;

    while (true)
    {
        xQueueReceive(SPI_display_gate_keeper.graphic_widget_data, &received_data_to_show, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        SPI_display_gate_keeper.receive_widget_data(received_data_to_show);
        if (probe != NULL)
            ((Probe *)probe)->lo();
    }
}

void SPI_graph_widget_task(void *probe)
{
    SPI_display_gate_keeper.send_clear_device_command(&color_display);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_rtos_graph_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&my_rtos_graph_widget);
    }
}

void SPI_values_widget_task(void *probe)
{
    my_text_widget title = my_text_widget(&color_display, common_title_config, ST7735_TEXT_CANVAS_FORMAT);
    title.writer->write("ROLL PITCH");
    SPI_display_gate_keeper.send_widget_data(&title);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_rtos_values_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&my_rtos_values_widget);
    }
}

void I2C_display_gate_keeper_task(void *probe)
{
    struct_WidgetDataToGateKeeper received_data_to_show;

    while (true)
    {
        xQueueReceive(I2C_display_gate_keeper.graphic_widget_data, &received_data_to_show, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        I2C_display_gate_keeper.receive_widget_data(received_data_to_show);
        if (probe != NULL)
            ((Probe *)probe)->lo();
    }
}

void I2C_right_graph_widget_task(void *probe)
{
    I2C_display_gate_keeper.send_clear_device_command(&right_display);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_rtos_right_graph_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        I2C_display_gate_keeper.send_widget_data(&my_rtos_right_graph_widget);
    }
}
void I2C_left_values_widget_task(void *probe)
{
    I2C_display_gate_keeper.send_clear_device_command(&left_display);
    my_text_widget left_title = my_text_widget(&left_display, common_title_config, SSD1306_CANVAS_FORMAT);
    left_title.writer->write("ROLL PITCH");
    I2C_display_gate_keeper.send_widget_data(&left_title);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_rtos_left_values_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        I2C_display_gate_keeper.send_widget_data(&my_rtos_left_values_widget);
    }
}

int main(){
    stdio_init_all();

    xTaskCreate(idle_task, "idle_task", 256, &p0, 0, NULL);

    xTaskCreate(my_model_task, "main_task", 256, &p1, 15, NULL); // 4us pour SPI_graph_widget_task, 12us SPI_values_widget_task, I2C_right_graph_widget_task, 16us pour I2C_left_values_widget_task

    xTaskCreate(I2C_left_values_widget_task, "left_values_widget_task", 256, &p4, 10, &my_rtos_left_values_widget.task_handle);// 2.69ms + 6.5ms xfer I2C
    xTaskCreate(SPI_values_widget_task, "values_widget_task", 256, &p4, 10, &my_rtos_values_widget.task_handle); // durée 5,6 ms + 3,8ms xfer SPI
    
    xTaskCreate(I2C_right_graph_widget_task, "right_graph_widget_task", 256, &p5, 10, &my_rtos_right_graph_widget.task_handle);// 368us + 22.2ms xfer I2C
    xTaskCreate(SPI_graph_widget_task, "graph_widget_task", 256, &p5, 10, &my_rtos_graph_widget.task_handle);    // durée: 8.23ms + 14ms xfer SPI

    xTaskCreate(SPI_display_gate_keeper_task, "SPI_gate_keeper_task", 256, &p6, 5, NULL);
    xTaskCreate(I2C_display_gate_keeper_task, "I2C_gate_keeper_task", 256, &p7, 5, NULL);

    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
