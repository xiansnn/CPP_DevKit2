/**
 * @file test_rtos_ssd1306_graphic_primitives.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-10-21
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

#include "device/SSD1306/ssd1306.h"
#include "font/raspberry26x32.h"
#include "sw/widget/rtos_widget.h"

#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p4 = Probe(4);

// Probe p5 = Probe(5);
// Probe p6 = Probe(6);
// Probe p7 = Probe(7);

#define INTER_TASK_DELAY 100
#define INTRA_TASK_DELAY 25

//==========================display gatekeeper===============
rtos_GraphicDisplayGateKeeper display_gate_keeper = rtos_GraphicDisplayGateKeeper();
//==========================Master I2C===========================
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
//=============================rtos_SSD1306=====================
struct_ConfigSSD1306 cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};
rtos_SSD1306 display = rtos_SSD1306(&master, cfg_ssd1306);
//====================== full SSD1306 graphic configuration====================
struct_ConfigGraphicWidget full_SSD1306_graphic_widget_cfg{
    .canvas_width_pixel = SSD1306_WIDTH,
    .canvas_height_pixel = SSD1306_HEIGHT,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0};
//=========================dummy rtos_GraphicWidget for the whole screen===================
class my_full_screen_widget : public rtos_GraphicWidget
{
private:
public:
    my_full_screen_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                          struct_ConfigGraphicWidget graph_cfg)
        : rtos_GraphicWidget(nullptr, graph_cfg, CanvasFormat::MONO_VLSB, graphic_display_screen) {};
    ~my_full_screen_widget() {};
    void get_value_of_interest() {};
    void draw() {};
};
//=====================================TASKS===================================
void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}
//----------------------------------------
void test_outofframe_line(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_SSD1306_graphic_widget_cfg);

    int y0, x1, y1;
    display_gate_keeper.send_clear_device_command(display);
    x1 = 64;
    y1 = 70;
    y0 = -10;

    for (int x = -10; x < 138; x++)
    {
        ColorIndex c = ColorIndex::WHITE;
        frame.drawer->line(x, y0, x1, y1, c);
        display_gate_keeper.send_widget_data(&frame);
        c = ColorIndex::BLACK;
        frame.drawer->line(x, y0, x1, y1, c);
        display_gate_keeper.send_widget_data(&frame);
    }
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
};
//--------------------------------------------------------------------------------------
void test_fb_line(rtos_SSD1306 *display)
{
    p1.hi();
    display_gate_keeper.send_clear_device_command(display);
    my_full_screen_widget frame = my_full_screen_widget(display, full_SSD1306_graphic_widget_cfg);
    ColorIndex c = ColorIndex::BLACK;
    for (int i = 0; i < 2; i++)
    {
        if (c == ColorIndex::BLACK)
            c = ColorIndex::WHITE;
        else
            c = ColorIndex::BLACK;

        for (int x = 0; x < SSD1306_WIDTH; x++)
        {
            frame.drawer->line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            display_gate_keeper.send_widget_data(&frame);
        }

        for (int y = SSD1306_HEIGHT - 1; y >= 0; y--)
        {
            frame.drawer->line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            display_gate_keeper.send_widget_data(&frame);
        }
    }
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    struct_RenderArea full_screen_area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    for (int i = 0; i < 2; i++)
    {
        for (int x = 0; x < SSD1306_WIDTH; x++)
        {
            c = ColorIndex::WHITE;
            frame.drawer->line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            display_gate_keeper.send_widget_data(&frame);
            c = ColorIndex::BLACK;
            frame.drawer->line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            display_gate_keeper.send_widget_data(&frame);
        }
        for (int y = SSD1306_HEIGHT - 1; y >= 0; y--)
        {
            c = ColorIndex::WHITE;
            frame.drawer->line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            display->show_render_area(frame.drawer->canvas->canvas_buffer, full_screen_area); ///////////////////
            c = ColorIndex::BLACK;
            frame.drawer->line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            display->show_render_area(frame.drawer->canvas->canvas_buffer, full_screen_area); /////////////////
        }
    }
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
};
//-------------------------------------------------------------------------------
void test_fb_hline(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_SSD1306_graphic_widget_cfg);

    display_gate_keeper.send_clear_device_command(display);

    frame.drawer->hline(0, 0, 32);
    display_gate_keeper.send_widget_data(&frame);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.drawer->hline(0, 15, 64);
    display_gate_keeper.send_widget_data(&frame);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.drawer->hline(0, 31, 96);
    display_gate_keeper.send_widget_data(&frame);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.drawer->hline(0, 47, 128);
    frame.drawer->hline(0, 63, 128);
    display_gate_keeper.send_widget_data(&frame);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}
//------------------------------------------------------------------------------------
void test_fb_vline(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_SSD1306_graphic_widget_cfg);

    display_gate_keeper.send_clear_device_command(display);
    frame.drawer->vline(0, 0, 16);
    display_gate_keeper.send_widget_data(&frame);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.drawer->vline(15, 0, 32);
    display_gate_keeper.send_widget_data(&frame);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.drawer->vline(31, 0, 48);
    display_gate_keeper.send_widget_data(&frame);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.drawer->vline(64, 0, 64);
    frame.drawer->vline(127, 0, 64);
    display_gate_keeper.send_widget_data(&frame);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}
//---------------------------------------------------------------------------------
void test_fb_rect(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_SSD1306_graphic_widget_cfg);

    display_gate_keeper.send_clear_device_command(display);
    frame.drawer->rect(0, 0, 128, 64);
    display_gate_keeper.send_widget_data(&frame);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.drawer->rect(10, 10, 108, 44, true);
    display_gate_keeper.send_widget_data(&frame);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}
//------------------------------------------------------------------------------------
void test_fb_in_fb(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_SSD1306_graphic_widget_cfg);

    display_gate_keeper.send_clear_device_command(display);
    frame.drawer->rect(0, 0, SSD1306_WIDTH, SSD1306_HEIGHT);
    frame.drawer->rect(10, 10, 108, 44, true);
    frame.drawer->line(5, 60, 120, 5, ColorIndex::BLACK);
    display_gate_keeper.send_widget_data(&frame);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));

    struct_ConfigGraphicWidget small_frame_cfg{
        .canvas_width_pixel = 80,
        .canvas_height_pixel = 24,
        .widget_anchor_x = 24,
        .widget_anchor_y = 24};

    my_full_screen_widget small_frame = my_full_screen_widget(display, small_frame_cfg);
    small_frame.drawer->canvas->clear_canvas_buffer();
    small_frame.drawer->line(5, 5, 80, 20);
    small_frame.drawer->circle(8, 44, 12);
    display_gate_keeper.send_widget_data(&small_frame);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}
//----------------------------------------------------------------------------------------
void test_fb_circle(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_SSD1306_graphic_widget_cfg);

    display_gate_keeper.send_clear_device_command(display);
    frame.drawer->circle(50, 63, 31);
    display_gate_keeper.send_widget_data(&frame);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.drawer->circle(20, 64, 32, true);
    display_gate_keeper.send_widget_data(&frame);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}
//------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------------------
void main_task(void *display_device)
{
    while (true)
    {
        test_fb_line((rtos_SSD1306 *)display_device);
        test_outofframe_line((rtos_SSD1306 *)display_device);
        test_fb_hline((rtos_SSD1306 *)display_device);
        test_fb_vline((rtos_SSD1306 *)display_device);
        test_fb_rect((rtos_SSD1306 *)display_device);
        test_fb_circle((rtos_SSD1306 *)display_device);
        test_fb_in_fb((rtos_SSD1306 *)display_device);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{

    stdio_init_all();

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(main_task, "main_task", 256, &display, 2, NULL);
    xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 256, NULL, 4, NULL);

    vTaskStartScheduler();

    while (true)
        tight_loop_contents();
}
