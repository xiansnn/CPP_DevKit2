/**
 * @file test_st7735_device.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-03-30
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
//=========================================================================
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
    .dc_pin = 14
    // .rotation = ST7735Rotation::_90
};

struct_ConfigGraphicWidget device_frame_cfg = {
    .pixel_frame_width = 128,
    .pixel_frame_height = 128,
    .fg_color = ColorIndex::WHITE,
    .bg_color = ColorIndex::BLACK,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = false};

struct_ConfigGraphicWidget corner_rectangle_cfg = {
    .pixel_frame_width = 16,
    .pixel_frame_height = 16,
    // .fg_color = ColorIndex::WHITE,
    .bg_color = ColorIndex::GRAY,
    .widget_anchor_x = 1,
    .widget_anchor_y = 1,
    .widget_with_border = true};

//===============================================================================
class my_corner_rectangle_widget : public GraphicWidget
{
private:
public:
    my_corner_rectangle_widget(GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigGraphicWidget graph_cfg, CanvasFormat format);
    ~my_corner_rectangle_widget();
    void get_value_of_interest();
    void draw();
};
my_corner_rectangle_widget::my_corner_rectangle_widget(GraphicDisplayDevice *graphic_display_screen,
                                                       struct_ConfigGraphicWidget graph_cfg, CanvasFormat format)
    : GraphicWidget(graphic_display_screen, graph_cfg, format) {}
my_corner_rectangle_widget::~my_corner_rectangle_widget() {}
void my_corner_rectangle_widget::get_value_of_interest() {}
void my_corner_rectangle_widget::draw()
{
    draw_border(fg_color);
};

//====================================================================================
class my_central_rectangle_widget : public GraphicWidget
{
private:
public:
    my_central_rectangle_widget(GraphicDisplayDevice *graphic_display_screen,
                                struct_ConfigGraphicWidget graph_cfg, CanvasFormat format);
    ~my_central_rectangle_widget();
    void get_value_of_interest();
    void draw();
};
my_central_rectangle_widget::my_central_rectangle_widget(GraphicDisplayDevice *graphic_display_screen, struct_ConfigGraphicWidget graph_cfg, CanvasFormat format)
    : GraphicWidget(graphic_display_screen, graph_cfg, format) {}
my_central_rectangle_widget::~my_central_rectangle_widget() {}
void my_central_rectangle_widget::get_value_of_interest() {}
void my_central_rectangle_widget::draw()
{
    rect(0, 0, this->canvas->canvas_width_pixel, this->canvas->canvas_height_pixel, false, fg_color);
};
//================================================================================
void test_clear_device_screen(ST7735 &display)
{
    for (auto &&i : color565_palette)
    {
        display.clear_device_screen_buffer(i.first);
        sleep_ms(1000);
    }
}
//================================================================================
void rotate_widget(ST7735 *display,
                   GraphicWidget *widget,
                   ColorIndex color,
                   ST7735Rotation rotation)
{
    display->set_rotation_and_color(rotation);
    widget->fg_color = color;
    widget->draw_border(color);
    widget->draw();
    widget->show();
}
//================================================================================

void test_screen_rotation(ST7735 *display)
{
    my_corner_rectangle_widget *corner_rectangle = new my_corner_rectangle_widget(display, corner_rectangle_cfg, CANVAS_FORMAT);
    my_central_rectangle_widget *central_rectangle = new my_central_rectangle_widget(display, device_frame_cfg, CANVAS_FORMAT);

    display->clear_device_screen_buffer();
    central_rectangle->draw();
    central_rectangle->show();

    for (size_t i = 0; i < 4; i++)
    {
        ST7735Rotation rotation = static_cast<ST7735Rotation>(i);
        ColorIndex color = static_cast<ColorIndex>(i + 2);
        rotate_widget(display, corner_rectangle, color, rotation);
    }
    sleep_ms(1000);

    delete corner_rectangle, central_rectangle;
}
//================================================================================

int main()
{

    stdio_init_all();
    HW_SPI_Master spi_master = HW_SPI_Master(cfg_spi);
    ST7735 display = ST7735(&spi_master, cfg_st7735);

    while (true)
    {
        test_screen_rotation(&display);
    }
    return 0;
}
