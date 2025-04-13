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
#include "sw/widget/widget.h"

#define CANVAS_FORMAT CanvasFormat::RGB565

struct_ConfigMasterSPI cfg_spi = {
    .spi = spi1,
    .sck_pin = 10,
    .tx_pin = 11,
    .rx_pin = 12,
    .cs_pin = 13,
    .baud_rate_Hz = 10 * 1000 * 1000};

struct_ConfigST7735 cfg_st7735{
    .display_type = ST7735DisplayType::ST7735_144_128_RGB_128_GREENTAB,
    .backlight_pin = 5,
    .hw_reset_pin = 15,
    .dc_pin = 14,
    .rotation = ST7735Rotation::_90};

struct_ConfigGraphicWidget full_screen_cfg = {
    .pixel_frame_width = 128,
    .pixel_frame_height = 128,
    .fg_color = ColorIndex::RED,
    .bg_color = ColorIndex::YELLOW,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};

class my_full_screen_widget : public GraphicWidget
{
private:
public:
    my_full_screen_widget(GraphicDisplayDevice *graphic_display_screen,
                          struct_ConfigGraphicWidget graph_cfg, CanvasFormat format);
    ~my_full_screen_widget();
    void get_value_of_interest();
    void draw();
};
my_full_screen_widget::my_full_screen_widget(GraphicDisplayDevice *graphic_display_screen,
                                             struct_ConfigGraphicWidget graph_cfg, CanvasFormat format)
    : GraphicWidget(graphic_display_screen, graph_cfg, format) { printf("+ my_full_screen_widget\n"); }
my_full_screen_widget::~my_full_screen_widget() { printf("- my_full_screen_widget\n"); }
void my_full_screen_widget::get_value_of_interest() {}
void my_full_screen_widget::draw() {};

void test_fb_line(ST7735 *display)
{
    display->clear_device_screen_buffer();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    for (int i = 2; i < 5; i++)
    {
        ColorIndex c = static_cast<ColorIndex>(i);

        for (int x = 0; x < 128; x++)
        {
            frame.line(x, 0, 128 - 1 - x, 128 - 1, c);
            frame.show();
        }

        for (int y = 128 - 1; y >= 0; y--)
        {
            frame.line(0, y, 128 - 1, 128 - 1 - y, c);
            frame.show();
        }
    }
}
void test_outofframe_line(ST7735 *display)
{
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    int y0, x1, y1;
    display->clear_device_screen_buffer();
    x1 = 64;
    y1 = 70;
    y0 = -10;

    uint8_t i = 0;

    for (int x = -10; x < 138; x++)
    {
        i++;
        if (i > 20)
            i = 0;
        ColorIndex c = static_cast<ColorIndex>(i);
        frame.line(x, y0, x1, y1, c);
        frame.show();
    }
}
void test_fb_rect(ST7735 *display)
{
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);

    display->clear_device_screen_buffer();
    frame.rect(0, 0, 128, 64);
    frame.show();
    sleep_ms(1000);
    frame.rect(10, 10, 108, 44, true);
    frame.show();
    sleep_ms(2000);
}
void test_fb_hline(ST7735 *display)
{
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);

    display->clear_device_screen_buffer(); // 50ms @ bitrate 10Mbps
    for (size_t i = 0; i < 16; i++)
    {
        frame.hline(0, i * 8, 128, static_cast<ColorIndex>(i + 1));
        // frame.show();
        // sleep_ms(100);
    }
    frame.show(); // 80ms @ bitrate 10Mbps
    sleep_ms(500);
}

int main()
{
    stdio_init_all();
    HW_SPI_Master spi_master = HW_SPI_Master(cfg_spi);
    ST7735 display = ST7735(&spi_master, cfg_st7735);
    display.clear_device_screen_buffer();

    while (true)
    {
        // test_fb_line(&display);
        // test_outofframe_line(&display);
        test_fb_hline(&display);
        // test_fb_vline(&display);
        // test_fb_rect(&display);
        // test_fb_circle(&display);
        // test_fb_in_fb(&display);
    }
    return 0;
}
