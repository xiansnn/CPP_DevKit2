/**
 * @file test_ssd1306.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief This file contains a set of functions that exercice the main feautures of the OLED display SSD1306.
 * Test functions are more or less are derived from https://github.com/Harbys/pico-ssd1306 works.
 * @version 0.1
 * @date 2024-08-05
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ssd1306.h"
#include "framebuffer.h"
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

/**
 * @brief test contrast command.
 *
 * repeat 3 times [contrast 0, contrast 255, contrast 127]
 *
 * @param display
 */
void test_contrast(SSD1306 *display)
{
    display->clear_device_screen_buffer();
    struct_RenderArea area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    display->fill_pattern_and_show_GDDRAM(0x55, area);
    area = SSD1306::compute_render_area(32, 96, 16, 32);
    display->fill_pattern_and_show_GDDRAM(0xFF, area);
    for (size_t i = 0; i < 3; i++)
    {
        display->set_contrast(0);
        sleep_ms(1000);
        display->set_contrast(255);
        sleep_ms(1000);
        display->set_contrast(127);
        sleep_ms(1000);
    }
};

/**
 * @brief  test addressing mode.
 * successsive test[horizontal addressing mode, verticale addressing mode, page addressing mode]
 *
 * @param display
 */
void test_addressing_mode(SSD1306 *display)
{
    uint8_t image[128 * 8]{0x00};
    memset(image, 0xFE, sizeof(image));
    sleep_ms(1000);
    display->clear_device_screen_buffer();
    struct_RenderArea area;
    // HORIZONTAL_ADDRESSING_MODE
    for (size_t i = 0; i < 4; i++)
    {
        memset(image, 0xAA, sizeof(image));
        area = SSD1306::compute_render_area(10 * i, 90 + 10 * i, 8 * i, 2 + 8 * i);
        display->show_render_area(image, area, HORIZONTAL_ADDRESSING_MODE);
        sleep_ms(1000);
        display->clear_device_screen_buffer();
    }
    // VERTICAL_ADDRESSING_MODE
    for (size_t i = 0; i < 4; i++)
    {
        memset(image, 0xAA, sizeof(image));
        area = SSD1306::compute_render_area(40 + 10 * i, 50 + 10 * i, 8 * i, 30 + 8 * i);
        display->show_render_area(image, area, VERTICAL_ADDRESSING_MODE);
        sleep_ms(1000);
        display->clear_device_screen_buffer();
    }
    // PAGE_ADDRESSING_MODE
    for (size_t i = 0; i < 8; i++)
    {
        memset(image, 0x55, sizeof(image));
        area = SSD1306::compute_render_area(i * 10, 100 + i * 10, 8 * i, 8 * i);
        display->show_render_area(image, area, PAGE_ADDRESSING_MODE);
        sleep_ms(1000);
    }
};

/**
 * @brief test blink command
 *
 * @param display
 */
void test_blink(SSD1306 *display)
{
    struct_RenderArea area;
    display->clear_device_screen_buffer();
    area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    display->fill_pattern_and_show_GDDRAM(0x81, area);
    area = SSD1306::compute_render_area(64, 96, 15, 40);
    display->fill_pattern_and_show_GDDRAM(0x7E, area);
    for (int i = 0; i < 2; i++)
    {
        display->set_all_pixel_ON();
        sleep_ms(1000);
        display->set_display_from_RAM();
        sleep_ms(1000);
    }
};
/**
 * @brief tst auto scrolling function of the SSD1306 device
 *
 * @param display
 */
void test_scrolling(SSD1306 *display)
{
    display->clear_device_screen_buffer();
    // render 3 cute little raspberries
    struct_RenderArea area = SSD1306::compute_render_area(0, IMG_WIDTH - 1, 0, IMG_HEIGHT - 1);
    uint8_t offset = 5 + IMG_WIDTH; // 5px padding
    for (int i = 0; i < 3; i++)
    {
        display->show_render_area(raspberry26x32, area);
        area.start_col += offset;
        area.end_col += offset;
    }
    // start scrolling
    struct_ConfigScrollSSD1306 scroll_data = {
        .time_frame_interval = _25_FRAMES,
        .vertical_scrolling_offset = 1};
    display->horizontal_scroll(true, scroll_data);
    sleep_ms(3000);
    display->horizontal_scroll(false, scroll_data);
    sleep_ms(1000);
    display->vertical_scroll(true, scroll_data);
    sleep_ms(5000);
    display->vertical_scroll(false, scroll_data);
};



int main()
{

    stdio_init_all();
    // create I2C bus hw peripheral and display
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    while (true)
    {
        test_blink(&display);
        test_contrast(&display);
        test_addressing_mode(&display);
        test_scrolling(&display);
    }
    return 0;
}
