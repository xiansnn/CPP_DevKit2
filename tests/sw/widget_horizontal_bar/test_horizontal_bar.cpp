/**
 * @file test_horizontal_bar.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "test_horizontal_bar_models.h"
#include "test_horizontal_bar_widgets.h"
#include "device/SSD1306/ssd1306.h"

#include "utilities/probe/probe.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

#define MAX_VALUE 10
#define MIN_VALUE -10
#define CANVAS_FORMAT CanvasFormat::MONO_VLSB

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


int main()
{
    pr_D4.hi();
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    struct_ConfigGraphicWidget controlled_horizontal_bar_cfg = {
        .canvas_width_pixel = 100,
        .canvas_height_pixel = 8,
        .canvas_foreground_color = ColorIndex::WHITE,
        .canvas_background_color = ColorIndex::BLACK,
        .widget_anchor_x = 20,
        .widget_anchor_y = 8,
        .widget_with_border = true};
    struct_ConfigGraphicWidget simple_horizontal_bar_cfg = {
        .canvas_width_pixel = 100,
        .canvas_height_pixel = 8,
        .canvas_foreground_color = ColorIndex::WHITE,
        .canvas_background_color = ColorIndex::BLACK,
        .widget_anchor_x = 20,
        .widget_anchor_y = 32,
        .widget_with_border = true};

    MyControlledHorizontalBarModel my_model = MyControlledHorizontalBarModel(MIN_VALUE, MAX_VALUE, true, 1);

    MySimpleHorizontalBarModel my_simple_model = MySimpleHorizontalBarModel(MIN_VALUE, MAX_VALUE, true, 1);

    MyControlledHorizontalBarWidget my_horizontal_bar = MyControlledHorizontalBarWidget(&my_model, &display,
                                                                                        MAX_VALUE, MIN_VALUE,
                                                                                        controlled_horizontal_bar_cfg,CANVAS_FORMAT);
    MySimpleHorizontalBarWidget my_simple_horizontal_bar = MySimpleHorizontalBarWidget(&my_simple_model, &display,
                                                                                       MAX_VALUE, MIN_VALUE,
                                                                                       simple_horizontal_bar_cfg,CANVAS_FORMAT);

    display.clear_device_screen_buffer();

    pr_D4.lo();

    while (true)
    {
        pr_D5.hi();

        my_model.increment_value();
        my_model.draw_refresh_all_attached_widgets();

        my_simple_model.increment_value();
        my_simple_model.draw_refresh_all_attached_widgets();

        pr_D5.lo();
        sleep_ms(500);
    }

    return 0;
}
