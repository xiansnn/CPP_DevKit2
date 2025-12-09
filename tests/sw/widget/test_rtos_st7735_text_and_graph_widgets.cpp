/**
 * @file test_rtos_st7735_text_and_graph_widgets.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "t_rtos_st7735_text_and_graph_widgets.h"
#include "sw/ui_core/rtos_ui_core.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

#define REFRESH_PERIOD_ms 50

#define GRAPHICS_CANVAS_FORMAT CanvasFormat::RGB565_16b
#define TEXT_CANVAS_FORMAT CanvasFormat::RGB565_16b

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
rtos_GraphicDisplayGateKeeper display_gate_keeper = rtos_GraphicDisplayGateKeeper();
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
    p7.hi();
    spi_master.spi_tx_dma_isr();
    p7.lo();
}
//================== ST7735===================
struct_ConfigST7735 cfg_st7735{
    .display_type = DEVICE_DISPLAY_TYPE,
    .backlight_pin = 5,
    .hw_reset_pin = 15,
    .dc_pin = 14,
    .rotation = DEVICE_DISPLAY_ROTATION};
rtos_ST7735 display = rtos_ST7735(&spi_master, cfg_st7735);

//-------------Model-----
my_model my_rtos_model = my_model();
//-------------Title-text-----
struct_ConfigTextWidget title_config = {
    .number_of_column = 10,
    .number_of_line = 1,
    .widget_anchor_x = 0,
    .widget_anchor_y = 64,
    .font = font_12x16};
my_text_widget title = my_text_widget(&display, title_config, TEXT_CANVAS_FORMAT);
//--------------Values-text----
struct_ConfigTextWidget values_config = {
    .number_of_column = 10,
    .number_of_line = 1,
    .widget_anchor_x = 0,
    .widget_anchor_y = (uint8_t)(title_config.widget_anchor_y + 2 * title_config.font[FONT_HEIGHT_INDEX]),
    .font = font_12x16};
my_text_widget my_rtos_values_widget = my_text_widget(&display, values_config, TEXT_CANVAS_FORMAT, &my_rtos_model);
//---------------Graph------------
struct_ConfigGraphicWidget graph_config{
    .canvas_width_pixel = 128,
    .canvas_height_pixel = 56,
    .canvas_foreground_color = ColorIndex::CYAN,
    .canvas_background_color = ColorIndex::MAROON,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};
my_visu_widget my_rtos_graph_widget = my_visu_widget(&display, graph_config, GRAPHICS_CANVAS_FORMAT, &my_rtos_model);
//---------------TASKS-----------------
void idle_task(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void display_gate_keeper_task(void *probe)
{
    struct_WidgetDataToGateKeeper received_data_to_show;

    while (true)
    {
        xQueueReceive(display_gate_keeper.graphic_widget_data, &received_data_to_show, portMAX_DELAY);
        ((Probe *)probe)->hi();
        display_gate_keeper.receive_widget_data(received_data_to_show);
        ((Probe *)probe)->lo();
    }
}

void my_model_task(void *pxProbe)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    my_rtos_model.update_attached_rtos_widget(&my_rtos_graph_widget);
    my_rtos_model.update_attached_rtos_widget(&my_rtos_values_widget);
    display_gate_keeper.send_clear_device_command(&display);
    my_text_widget title = my_text_widget(&display, title_config, TEXT_CANVAS_FORMAT);
    title.writer->write("ROLL PITCH");
    display_gate_keeper.send_widget_data(&title);

    ((Probe *)pxProbe)->pulse_us(100);
    int sign = 1;
    while (true)
    {
        sign *= -1;
        for (int i = -90; i < 90; i += 10)
        {
            ((Probe *)pxProbe)->hi();
            my_rtos_model.update_cycle(i, sign); //  cyclic_computation((Probe *)pxProbe);
            my_rtos_model.notify_all_linked_widget_task();
            ((Probe *)pxProbe)->lo();
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(REFRESH_PERIOD_ms));
        }
    }
}
void values_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        p3.hi();
        my_rtos_values_widget.draw();
        p3.lo();
        display_gate_keeper.send_widget_data(&my_rtos_values_widget);
        p3.pulse_us(10);
    }
}
void graph_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        p2.hi();
        my_rtos_graph_widget.draw();
        p2.lo();
        display_gate_keeper.send_widget_data(&my_rtos_graph_widget);
        p2.pulse_us(10);
    }
}

int main()

{
    stdio_init_all();

    xTaskCreate(idle_task, "idle_task", 256, &p0, 0, NULL);

    xTaskCreate(my_model_task, "main_task", 256, &p1, 15, NULL);
    xTaskCreate(graph_widget_task, "widget_task_1", 256, NULL, 10, &my_rtos_graph_widget.task_handle);
    xTaskCreate(values_widget_task, "widget_task_2", 256, NULL, 10, &my_rtos_values_widget.task_handle);

    xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 256, &p6, 5, NULL);

    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
