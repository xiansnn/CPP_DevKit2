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
#include "sw/widget/widget.h"
#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p4 = Probe(4);

// Probe p5 = Probe(5);
// Probe p6 = Probe(6);
// Probe p7 = Probe(7);

#define INTER_TASK_DELAY 1000
#define INTRA_TASK_DELAY 250

QueueHandle_t display_data_queue = xQueueCreate(8, sizeof(struct_DataToShow));
SemaphoreHandle_t data_sent = xSemaphoreCreateBinary(); // synchro between display task and sending task

void i2c_irq_handler();

struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE,
    .i2c_tx_master_handler = i2c_irq_handler};

struct_ConfigSSD1306 cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

rtos_HW_I2C_Master master = rtos_HW_I2C_Master(cfg_i2c);
rtos_SSD1306 display = rtos_SSD1306(&master, cfg_ssd1306);

void i2c_irq_handler()
{
    master.i2c_dma_isr();
};

struct_ConfigGraphicWidget SSD1306_framebuffer_cfg{
    .pixel_frame_width = SSD1306_WIDTH,
    .pixel_frame_height = SSD1306_HEIGHT,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0};

class my_full_screen_widget : public GraphicWidget
{
private:
public:
    my_full_screen_widget(GraphicDisplayDevice *graphic_display_screen,
                          struct_ConfigGraphicWidget graph_cfg);
    ~my_full_screen_widget();
    void get_value_of_interest();
    void draw();
};
my_full_screen_widget::my_full_screen_widget(GraphicDisplayDevice *graphic_display_screen,
                                             struct_ConfigGraphicWidget graph_cfg)
    : GraphicWidget(graphic_display_screen, graph_cfg, CanvasFormat::MONO_VLSB) {}
my_full_screen_widget::~my_full_screen_widget() {}
void my_full_screen_widget::get_value_of_interest() {}
void my_full_screen_widget::draw() {}

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

/**
 * @brief  Check that we can draw a line that outfit the framebuffer without consequences
 *
 * @param display
 */
void test_outofframe_line(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, SSD1306_framebuffer_cfg);
    int y0, x1, y1;
    display->send_clear_device_command(display_data_queue, data_sent);
    x1 = 64;
    y1 = 70;
    y0 = -10;

    for (int x = -10; x < 138; x++)
    {
        ColorIndex c = ColorIndex::WHITE;
        frame.line(x, y0, x1, y1, c);
        frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
        c = ColorIndex::BLACK;
        frame.line(x, y0, x1, y1, c);
        frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    }
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
};
/**
 * @brief test framebuffer line function
 *
 * @param display
 */
void test_fb_line(rtos_SSD1306 *display)
{
    p1.hi();
    display->send_clear_device_command(display_data_queue, data_sent);
    my_full_screen_widget frame = my_full_screen_widget(display, SSD1306_framebuffer_cfg);
    ColorIndex c = ColorIndex::BLACK;
    for (int i = 0; i < 2; i++)
    {
        if (c == ColorIndex::BLACK)
            c = ColorIndex::WHITE;
        else
            c = ColorIndex::BLACK;

        for (int x = 0; x < SSD1306_WIDTH; x++)
        {
            frame.line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
        }

        for (int y = SSD1306_HEIGHT - 1; y >= 0; y--)
        {
            frame.line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
        }
    }

    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));

    struct_RenderArea full_screen_area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    for (int i = 0; i < 2; i++)
    {
        for (int x = 0; x < SSD1306_WIDTH; x++)
        {
            c = ColorIndex::WHITE;
            frame.line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
            c = ColorIndex::BLACK;
            frame.line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
        }
        for (int y = SSD1306_HEIGHT - 1; y >= 0; y--)
        {
            c = ColorIndex::WHITE;
            frame.line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            display->show_render_area(frame.canvas->canvas_buffer, full_screen_area); ///////////////////
            c = ColorIndex::BLACK;
            frame.line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            display->show_render_area(frame.canvas->canvas_buffer, full_screen_area); /////////////////
        }
    }
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
};

/**
 * @brief test framebuffer hline function
 *
 * @param display
 */
void test_fb_hline(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, SSD1306_framebuffer_cfg);

    display->send_clear_device_command(display_data_queue, data_sent);

    frame.hline(0, 0, 32);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    sleep_ms(1000);
    frame.hline(0, 15, 64);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    sleep_ms(1000);
    frame.hline(0, 31, 96);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    sleep_ms(1000);
    frame.hline(0, 47, 128);
    frame.hline(0, 63, 128);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}
/**
 * @brief test framebuffer vline function
 *
 * @param display
 */
void test_fb_vline(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, SSD1306_framebuffer_cfg);

    display->send_clear_device_command(display_data_queue, data_sent);
    frame.vline(0, 0, 16);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    sleep_ms(1000);
    frame.vline(15, 0, 32);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    sleep_ms(1000);
    frame.vline(31, 0, 48);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    sleep_ms(1000);
    frame.vline(64, 0, 64);
    frame.vline(127, 0, 64);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}

/**
 * @brief test framebuffer rect function
 *
 * @param display
 */
void test_fb_rect(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, SSD1306_framebuffer_cfg);

    display->send_clear_device_command(display_data_queue, data_sent);
    frame.rect(0, 0, 128, 64);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    sleep_ms(1000);
    frame.rect(10, 10, 108, 44, true);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}
/**
 * @brief test capability of drawing a framebuffer inside another framebuffer
 *
 * @param display
 */
void test_fb_in_fb(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, SSD1306_framebuffer_cfg);

    display->send_clear_device_command(display_data_queue, data_sent);
    frame.rect(0, 0, SSD1306_WIDTH, SSD1306_HEIGHT);
    frame.rect(10, 10, 108, 44, true);
    frame.line(5, 60, 120, 5, ColorIndex::BLACK);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));

    struct_ConfigGraphicWidget small_frame_cfg{
        .pixel_frame_width = 80,
        .pixel_frame_height = 24,
        .widget_anchor_x = 24,
        .widget_anchor_y = 24};

    my_full_screen_widget small_frame = my_full_screen_widget(display, small_frame_cfg);
    small_frame.canvas->clear_canvas_buffer();
    small_frame.line(5, 5, 80, 20);
    small_frame.circle(8, 44, 12);
    small_frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}

/**
 * @brief test framebuffer circle function
 *
 * @param display
 */
void test_fb_circle(rtos_SSD1306 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, SSD1306_framebuffer_cfg);

    display->send_clear_device_command(display_data_queue, data_sent);
    frame.circle(50, 63, 31);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.circle(20, 64, 32, true);
    frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}

void display_gate_keeper_task(void *param)
{
    struct_DataToShow received_data_to_show;

    while (true)
    {
        xQueueReceive(display_data_queue, &received_data_to_show, portMAX_DELAY);
        p4.hi();
        switch (received_data_to_show.command)
        {
        case DisplayCommand::show_image:
            ((rtos_SSD1306 *)received_data_to_show.display)->show_from_display_queue(received_data_to_show);
            break;
        case DisplayCommand::clear_screen:
            ((rtos_SSD1306 *)received_data_to_show.display)->clear_device_screen_buffer();
            break;
        default:
            break;
        }

        xSemaphoreGive(data_sent);
        p4.lo();
    }
}

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

int main()
{

    stdio_init_all();
    // create I2C bus hw peripheral and display

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(main_task, "main_task", 256, &display, 2, NULL);
    xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 256, NULL, 4, NULL);

    vTaskStartScheduler();

    while (true)
        tight_loop_contents();
}
