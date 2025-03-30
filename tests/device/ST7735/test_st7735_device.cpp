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
#include "utilities/probe/probe.h"

#define DEGREE "\xF8"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

struct_ConfigMasterSPI cfg_spi = {
    .spi = spi1,
    .sck_pin = 10,
    .tx_pin = 11,
    .rx_pin = 12,
    .cs_pin = 13,
    .baud_rate_Hz = 1000 * 1000};

struct_ConfigST7735 cfg_st7735{
    .device_width = 128,
    .device_height = 128,
    .dc_pin = 14,
    .backlight_pin = 5,
    .rotation = ST7735Rotation::_0,
    .hw_reset_pin = 15};

int main()
{

    stdio_init_all();
    HW_SPI_Master spi_master = HW_SPI_Master(cfg_spi);
    ST7735 display = ST7735(&spi_master, cfg_st7735);

    struct_ConfigGraphicWidget frame = {
        .pixel_frame_width = display.screen_pixel_width,
        .pixel_frame_height = display.screen_pixel_height,
        .fg_color = PixelColor::WHITE,
        .bg_color = PixelColor::BLACK,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .widget_with_border = false};

    while (true)
    {

    }
    return 0;
}

void test_display_on(ST7735 &display)
{
    display.display_on(true);
    sleep_ms(500);
    display.display_on(false);
    sleep_ms(500);
}
