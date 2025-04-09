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
    .rotation = ST7735Rotation::_0};

class my_graphic_widget : public GraphicWidget
{
private:
public:
    my_graphic_widget(GraphicDisplayDevice *graphic_display_screen,
                      struct_ConfigGraphicWidget graph_cfg,CanvasFormat format);
    ~my_graphic_widget();
    void get_value_of_interest();
    void draw();
};
my_graphic_widget::my_graphic_widget(GraphicDisplayDevice *graphic_display_screen,
                                     struct_ConfigGraphicWidget graph_cfg,CanvasFormat format)
    : GraphicWidget(graphic_display_screen, graph_cfg,format) {}
my_graphic_widget::~my_graphic_widget() {}
void my_graphic_widget::get_value_of_interest() {}
void my_graphic_widget::draw() {};

void test_clear_device_screen(ST7735 &display)
{
    for (auto &&i : color565_palette)
    {
        display.clear_device_screen_buffer(i.first);
        sleep_ms(1000);
    }
}

int main()
{

    stdio_init_all();
    HW_SPI_Master spi_master = HW_SPI_Master(cfg_spi);
    ST7735 display = ST7735(&spi_master, cfg_st7735);

    struct_ConfigGraphicWidget device_frame_cfg = {
        .pixel_frame_width = display.TFT_panel_width_in_pixel,
        .pixel_frame_height = display.TFT_panel_height_in_pixel,
        .fg_color = ColorIndex::WHITE,
        .bg_color = ColorIndex::BLACK,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .widget_with_border = true};

    my_graphic_widget full_screen = my_graphic_widget(&display, device_frame_cfg,CANVAS_FORMAT);

    while (true)
    {
        test_clear_device_screen(display);
    }
    return 0;
}
