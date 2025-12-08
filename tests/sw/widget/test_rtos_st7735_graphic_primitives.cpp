/**
 * @file test_st7735_graphic_primitives.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#include <numbers>
#include <sstream>
#include <iomanip>

#include "device/ST7735/st7735.h"

#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
// Probe p6 = Probe(6);
// Probe p7 = Probe(7);

#define CANVAS_FORMAT CanvasFormat::RGB565_16b

#define INTRA_TASK_DELAY 500
#define INTER_TASK_DELAY_ms 1000

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
    p3.hi();
    spi_master.spi_tx_dma_isr();
    p3.lo();
}
//================== ST7735===================
struct_ConfigST7735 cfg_st7735{
    .display_type = DEVICE_DISPLAY_TYPE,
    .backlight_pin = 5,
    .hw_reset_pin = 15,
    .dc_pin = 14,
    .rotation = DEVICE_DISPLAY_ROTATION,
};
rtos_ST7735 display = rtos_ST7735(&spi_master, cfg_st7735);
//===================full ST7735 graphic configuration===================
struct_ConfigGraphicWidget full_screen_cfg = {
    .canvas_width_pixel = 128,
    .canvas_height_pixel = DEVICE_DISPLAY_HEIGHT,
    .canvas_foreground_color = ColorIndex::RED,
    .canvas_background_color = ColorIndex::YELLOW,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};
//=========================dummy rtos_GraphicWidget for the whole screen===================
class my_full_screen_widget : public rtos_GraphicWidget
{
private:
public:
    my_full_screen_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                          struct_ConfigGraphicWidget graph_cfg, CanvasFormat format)
        : rtos_GraphicWidget(nullptr, graph_cfg, format, graphic_display_screen) {};
    ~my_full_screen_widget() {};
    void get_value_of_interest() {};
    void draw() {};
};
//===============================TASKS===================================================
void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void display_gate_keeper_task(void *param)
{

    struct_WidgetDataToGateKeeper received_data_to_show;

    while (true)
    {
        xQueueReceive(display_gate_keeper.graphic_widget_data, &received_data_to_show, portMAX_DELAY);
        p4.hi();
        display_gate_keeper.receive_widget_data(received_data_to_show);
        p4.lo();
    }
}

void test_fb_line(rtos_ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    p2.hi();
    display_gate_keeper.send_clear_device_command(display);
    p2.hi();
    frame.drawer->canvas->clear_canvas_buffer();
    p2.lo();
    p2.hi();
    int i = 0;
    for (int x = 0; x < 128; x++)
    {
        i++;
        if (i > 20 * 8)
            i = 1;
        frame.drawer->line(x, 0, 128 - 1 - x, 128 - 1, static_cast<ColorIndex>(i / 8));
        display_gate_keeper.send_widget_data(&frame);
    }
    p2.lo();
    p2.hi();

    for (int y = 128 - 1; y >= 0; y--)
    {
        i++;
        if (i > 20 * 8)
            i = 1;
        frame.drawer->line(0, y, 128 - 1, 128 - 1 - y, static_cast<ColorIndex>(i / 8));
        display_gate_keeper.send_widget_data(&frame);
    }
    p2.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}

void test_outofframe_line(rtos_ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    int y0, x1, y1;
    p2.hi();
    display_gate_keeper.send_clear_device_command(display);

    p2.lo();
    p2.hi();
    frame.drawer->canvas->clear_canvas_buffer();
    x1 = 64;
    y1 = 160;
    y0 = -10;

    uint8_t i = 0;
    p2.lo();
    p2.hi();
    for (int x = -10; x < 138; x++)
    {
        i++;
        if (i > 20 * 8)
            i = 0;
        ColorIndex c = static_cast<ColorIndex>(i / 8);
        frame.drawer->line(x, y0, x1, y1, c);
        display_gate_keeper.send_widget_data(&frame);
    }
    p2.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}
void test_fb_rect(rtos_ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    p2.hi();
    display_gate_keeper.send_clear_device_command(display);

    p2.lo();
    p2.hi();
    frame.drawer->canvas->clear_canvas_buffer();
    frame.drawer->rect(0, 0, 128, 64, false, ColorIndex::RED);
    display_gate_keeper.send_widget_data(&frame);
    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    p2.hi();
    frame.drawer->rect(10, 10, 108, 44, true, ColorIndex::YELLOW);
    display_gate_keeper.send_widget_data(&frame);
    p2.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}
void test_fb_hline(rtos_ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    p2.hi();
    display_gate_keeper.send_clear_device_command(display);

    p2.lo();
    p2.hi();
    frame.drawer->canvas->clear_canvas_buffer();

    for (size_t i = 0; i < 16; i++)
    {
        frame.drawer->hline(0, i * 8, 128, static_cast<ColorIndex>(i + 1));
        display_gate_keeper.send_widget_data(&frame);
    }
    display_gate_keeper.send_widget_data(&frame);
    p2.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}
void test_fb_vline(rtos_ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    p2.hi();
    display_gate_keeper.send_clear_device_command(display);

    p2.lo();
    p2.hi();

    frame.drawer->canvas->clear_canvas_buffer();

    for (size_t i = 0; i < 16; i++)
    {
        frame.drawer->vline(i * 8, 0, 128, static_cast<ColorIndex>(i + 1));
        display_gate_keeper.send_widget_data(&frame);
    }
    display_gate_keeper.send_widget_data(&frame);
    p2.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}

void test_fb_circle(rtos_ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    p2.hi();
    display_gate_keeper.send_clear_device_command(display);

    p2.lo();
    p2.hi();
    frame.drawer->canvas->clear_canvas_buffer();
    frame.drawer->circle(50, 63, 31, false, ColorIndex::ORANGE);
    display_gate_keeper.send_widget_data(&frame);
    p2.lo();
    p2.hi();
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.drawer->circle(20, 64, 32, true, ColorIndex::LIME);
    display_gate_keeper.send_widget_data(&frame);
    p2.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}

void test_fb_in_fb(rtos_ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    p2.hi();
    display_gate_keeper.send_clear_device_command(display);

    p2.lo();
    p2.hi();
    frame.drawer->canvas->clear_canvas_buffer();
    frame.drawer->rect(0, 0, display->TFT_panel_width_in_pixel, display->TFT_panel_height_in_pixel);
    frame.drawer->rect(10, 10, 108, 44, true, ColorIndex::CYAN);
    frame.drawer->line(5, 60, 120, 5, ColorIndex::RED);
    display_gate_keeper.send_widget_data(&frame);
    p2.lo();

    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    p2.hi();
    struct_ConfigGraphicWidget small_frame_cfg{
        .canvas_width_pixel = 80,
        .canvas_height_pixel = 24,
        .widget_anchor_x = 24,
        .widget_anchor_y = 24};
    my_full_screen_widget small_frame = my_full_screen_widget(display, small_frame_cfg, CANVAS_FORMAT);
    small_frame.drawer->canvas->fill_canvas_with_color(ColorIndex::NAVY);
    small_frame.drawer->line(5, 5, 80, 20, ColorIndex::YELLOW);
    small_frame.drawer->circle(8, 44, 12, false, ColorIndex::GREEN);
    display_gate_keeper.send_widget_data(&small_frame);
    p2.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}

void main_task(void *display_device)
{
    while (true)
    {
        test_fb_line((rtos_ST7735 *)display_device);
        test_outofframe_line((rtos_ST7735 *)display_device);
        test_fb_hline((rtos_ST7735 *)display_device);
        test_fb_vline((rtos_ST7735 *)display_device);
        test_fb_rect((rtos_ST7735 *)display_device);
        test_fb_circle((rtos_ST7735 *)display_device);
        test_fb_in_fb((rtos_ST7735 *)display_device);
    }
}

int main()
{
    p5.hi();
    stdio_init_all();

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(main_task, "main_task", 256, (void *)&display, 2, NULL);
    xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 250, NULL, 4, NULL);

    p5.lo();
    vTaskStartScheduler();

    while (true)
        tight_loop_contents();
    return 0;
}
