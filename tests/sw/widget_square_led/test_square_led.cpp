/**
 * @file test_square_led.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "device/SSD1306/ssd1306.h"
#include "utilities/probe/probe.h"
#include "t_switch_button_controller.cpp"
#include "t_square_led_widget.cpp"

#define CANVAS_FORMAT CanvasFormat::MONO_VLSB


/// @brief ########## Debug/Observer Probe for logic analyser section ##########
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
// Probe pr_D1 = Probe(1);

/// @brief ########## configuration section ##########

/// @brief define central switch config
struct_ConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};
#define CENTRAL_SWITCH_GPIO 6

/// @brief  define i2c config
struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};
/// @brief define SSD1306 display config
struct_ConfigSSD1306 cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

struct_ConfigGraphicWidget square_led_cfg = {
    .pixel_frame_width = 16,
    .pixel_frame_height = 16,
    .fg_color = ColorIndex::WHITE,
    .bg_color = ColorIndex::BLACK,
    .widget_anchor_x = 60,
    .widget_anchor_y = 32,
    .widget_with_border = true};

/**
 * @brief Example of main program of a SquareLED widget.
 *
 */
int main()
{
    pr_D4.hi();
    stdio_init_all();

    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    MySquareLedModel my_model = MySquareLedModel();

    my_blinking_square_led_widget square_led = my_blinking_square_led_widget(&my_model, &display, square_led_cfg,CANVAS_FORMAT);
    square_led.set_blink_us(300000);

    MySwitchButton central_switch = MySwitchButton(CENTRAL_SWITCH_GPIO, cfg_central_switch);
    central_switch.update_current_controlled_object(&my_model);

    display.clear_device_screen_buffer();

    pr_D4.lo();

    while (true)

    {
        pr_D5.hi();

        // UIControlEvent event = ((MySwitchButton *)my_model.get_current_controller())->process_sample_event(); // first alternative code
        UIControlEvent event = central_switch.process_sample_event(); // second alternative code
        my_model.process_control_event(event);

        square_led.draw();

        pr_D5.lo();

        sleep_ms(20);
    }

    return 0;
}