/**
 * @file test_graphic_framebuffer.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-02-02
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "ssd1306.h"
#include "widget.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "ssd1306/raspberry26x32.h"
#include <math.h>
#include <numbers>
#include "probe.h"
#include <sstream>
#include <iomanip>

#define DEGREE "\xF8"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};

struct_ConfigSSD1306 cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

struct_ConfigGraphicWidget SSD1306_framebuffer_cfg{
    .pixel_frame_width = SSD1306_WIDTH,
    .pixel_frame_height = SSD1306_HEIGHT,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0};

class my_graphic_widget : public GraphicWidget
{
private:
public:
    my_graphic_widget(GraphicDisplayDevice *graphic_display_screen,
                      struct_ConfigGraphicWidget graph_cfg);
    ~my_graphic_widget();
    void get_value_of_interest();
    void draw();
};
my_graphic_widget::my_graphic_widget(GraphicDisplayDevice *graphic_display_screen,
                                     struct_ConfigGraphicWidget graph_cfg)
    : GraphicWidget(graphic_display_screen, graph_cfg) {}
my_graphic_widget::~my_graphic_widget() {}
void my_graphic_widget::get_value_of_interest() {}
void my_graphic_widget::draw() {}

/**
 * @brief  Check that we can draw a line that outfit the framebuffer without consequences
 *
 * @param display
 */
void test_outofframe_line(SSD1306 *display)
{
    my_graphic_widget frame = my_graphic_widget(display, SSD1306_framebuffer_cfg);
    int y0, x1, y1;
    display->clear_device_screen_buffer();
    x1 = 64;
    y1 = 70;
    y0 = -10;

    for (int x = -10; x < 138; x++)
    {
        PixelColor c = PixelColor::WHITE;
        frame.line(x, y0, x1, y1, c);
        frame.show();
        c = PixelColor::BLACK;
        frame.line(x, y0, x1, y1, c);
        frame.show();
    }
};
/**
 * @brief test framebuffer line function
 *
 * @param display
 */
void test_fb_line(SSD1306 *display)
{
    display->clear_device_screen_buffer();
    my_graphic_widget frame = my_graphic_widget(display, SSD1306_framebuffer_cfg);
    PixelColor c = PixelColor::BLACK;
    struct_RenderArea full_screen_area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    for (int i = 0; i < 2; i++)
    {
        if (c == PixelColor::BLACK)
            c = PixelColor::WHITE;
        else
            c = PixelColor::BLACK;

        for (int x = 0; x < SSD1306_WIDTH; x++)
        {
            frame.line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            frame.show();
        }

        for (int y = SSD1306_HEIGHT - 1; y >= 0; y--)
        {
            frame.line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            frame.show();
        }
    }

    sleep_ms(1000);
    for (int i = 0; i < 2; i++)
    {
        for (int x = 0; x < SSD1306_WIDTH; x++)
        {
            c = PixelColor::WHITE;
            frame.line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            frame.show();
            c = PixelColor::BLACK;
            frame.line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            frame.show();
        }

        for (int y = SSD1306_HEIGHT - 1; y >= 0; y--)
        {
            c = PixelColor::WHITE;
            frame.line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            display->show_render_area(frame.pixel_frame.pixel_frame_buffer, full_screen_area);
            c = PixelColor::BLACK;
            frame.line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            display->show_render_area(frame.pixel_frame.pixel_frame_buffer, full_screen_area);
        }
    }
    sleep_ms(1000);
};

/**
 * @brief test framebuffer hline function
 *
 * @param display
 */
void test_fb_hline(SSD1306 *display)
{
    my_graphic_widget frame = my_graphic_widget(display, SSD1306_framebuffer_cfg);

    display->clear_device_screen_buffer();

    frame.hline(0, 0, 32);
    frame.show();
    sleep_ms(1000);
    frame.hline(0, 15, 64);
    frame.show();
    sleep_ms(1000);
    frame.hline(0, 31, 96);
    frame.show();
    sleep_ms(1000);
    frame.hline(0, 47, 128);
    frame.hline(0, 63, 128);
    frame.show();
    sleep_ms(1000);
}
/**
 * @brief test framebuffer vline function
 *
 * @param display
 */
void test_fb_vline(SSD1306 *display)
{
    my_graphic_widget frame = my_graphic_widget(display, SSD1306_framebuffer_cfg);

    display->clear_device_screen_buffer();
    frame.vline(0, 0, 16);
    frame.show();
    sleep_ms(1000);
    frame.vline(15, 0, 32);
    frame.show();
    sleep_ms(1000);
    frame.vline(31, 0, 48);
    frame.show();
    sleep_ms(1000);
    frame.vline(64, 0, 64);
    frame.vline(127, 0, 64);
    frame.show();
    sleep_ms(1000);
}

/**
 * @brief test framebuffer rect function
 *
 * @param display
 */
void test_fb_rect(SSD1306 *display)
{
    my_graphic_widget frame = my_graphic_widget(display, SSD1306_framebuffer_cfg);

    display->clear_device_screen_buffer();
    frame.rect(0, 0, 128, 64);
    frame.show();
    sleep_ms(1000);
    frame.rect(10, 10, 108, 44, true);
    frame.show();
    sleep_ms(2000);
}
/**
 * @brief test capability of drawing a framebuffer inside another framebuffer
 *
 * @param display
 */
void test_fb_in_fb(SSD1306 *display)
{
    my_graphic_widget frame = my_graphic_widget(display, SSD1306_framebuffer_cfg);

    display->clear_device_screen_buffer();
    frame.rect(0, 0, SSD1306_WIDTH, SSD1306_HEIGHT);
    frame.rect(10, 10, 108, 44, true);
    frame.line(5, 60, 120, 5, PixelColor::BLACK);
    frame.show();
    sleep_ms(1000);

    struct_ConfigGraphicWidget small_frame_cfg{
        .pixel_frame_width = 80,
        .pixel_frame_height = 24,
        .widget_anchor_x = 24,
        .widget_anchor_y = 24};

    my_graphic_widget small_frame = my_graphic_widget(display, small_frame_cfg);
    small_frame.fill(PixelColor::BLACK);
    small_frame.line(5, 5, 80, 20);
    small_frame.circle(8, 44, 12);
    small_frame.show();
    sleep_ms(1000);
}

/**
 * @brief test framebuffer circle function
 *
 * @param display
 */
void test_fb_circle(SSD1306 *display)
{
    my_graphic_widget frame = my_graphic_widget(display, SSD1306_framebuffer_cfg);

    display->clear_device_screen_buffer();
    frame.circle(50, 63, 31);
    frame.show();
    sleep_ms(1000);
    frame.circle(20, 64, 32, true);
    frame.show();
    sleep_ms(2000);
}

int main()
{

    stdio_init_all();
    // create I2C bus hw peripheral and display
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    while (true)
    {
        test_fb_line(&display);
        test_outofframe_line(&display);
        test_fb_hline(&display);
        test_fb_vline(&display);
        test_fb_rect(&display);
        test_fb_circle(&display);
        test_fb_in_fb(&display);
    }
    return 0;
}
