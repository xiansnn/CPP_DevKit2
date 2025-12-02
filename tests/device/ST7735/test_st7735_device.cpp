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

#define CANVAS_FORMAT CanvasFormat::RGB_COLOR_INDEX_8b
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



//=========================================================================
struct_ConfigMasterSPI cfg_spi = {
    .spi = spi1,
    .sck_pin = 10,
    .tx_pin = 11,
    .rx_pin = 12,
    .cs_pin = 13,
    .baud_rate_Hz = 10 * 1000 * 1000};
//-------------------
HW_SPI_Master spi_master = HW_SPI_Master(cfg_spi);

//=========================================================================
struct_ConfigST7735 cfg_st7735{
    .display_type = DEVICE_DISPLAY_TYPE,
    .backlight_pin = 5,
    .hw_reset_pin = 15,
    .dc_pin = 14,
    .rotation = DEVICE_DISPLAY_ROTATION};
//-------------------
ST7735 display = ST7735(&spi_master, cfg_st7735);

//===============================================================================
struct_ConfigGraphicWidget corner_rectangle_cfg = {
    .canvas_width_pixel = 16,
    .canvas_height_pixel = 16,
    .canvas_background_color = ColorIndex::GRAY,
    .widget_anchor_x = 1,
    .widget_anchor_y = 1,
    .widget_with_border = true};
//-----------------------
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
void my_corner_rectangle_widget::draw() {};

//====================================================================================
struct_ConfigGraphicWidget full_screen_cfg = {
    .canvas_width_pixel = 128,
    .canvas_height_pixel = DEVICE_DISPLAY_HEIGHT, 
    .canvas_foreground_color = ColorIndex::WHITE,
    .canvas_background_color = ColorIndex::BLACK,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};
//-----------------------
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
my_full_screen_widget::my_full_screen_widget(GraphicDisplayDevice *graphic_display_screen, struct_ConfigGraphicWidget graph_cfg, CanvasFormat format)
    : GraphicWidget(graphic_display_screen, graph_cfg, format) {}
my_full_screen_widget::~my_full_screen_widget() {}
void my_full_screen_widget::get_value_of_interest() {}
void my_full_screen_widget::draw() {};

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
void test_screen_rotation(ST7735 *display, GraphicWidget *central_rectangle, GraphicWidget *corner_rectangle)
{
    cfg_st7735.rotation = ST7735Rotation::_0;
    display->set_rotation_and_color(cfg_st7735);
    display->clear_device_screen_buffer();
    central_rectangle->draw_border();
    central_rectangle->show();

    for (size_t i = 0; i < 4; i++)
    {
        cfg_st7735.rotation = static_cast<ST7735Rotation>(i);
        display->set_rotation_and_color(cfg_st7735);
        corner_rectangle->canvas->fg_color = static_cast<ColorIndex>(i + 2);
        corner_rectangle->draw_border(corner_rectangle->canvas->fg_color);
        corner_rectangle->show();
    }
    sleep_ms(1000);
}
//================================================================================

int main()
{
    stdio_init_all();

    my_corner_rectangle_widget corner_rectangle = my_corner_rectangle_widget(&display, corner_rectangle_cfg, CANVAS_FORMAT);
    my_full_screen_widget central_rectangle = my_full_screen_widget(&display, full_screen_cfg, CANVAS_FORMAT);

    while (true)
    {
        test_screen_rotation(&display, &central_rectangle, &corner_rectangle);
    }
    return 0;
}
