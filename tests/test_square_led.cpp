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
#include "ssd1306.h"
#include "probe.h"

#include "test_square_led/t_switch_button_controller.cpp"
#include "test_square_led/t_square_led_widget.cpp"

/// @brief ########## Debug/Observer Probe for logic analyser section ##########
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D1 = Probe(1);

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

struct_ConfigGraphicFramebuffer square_led_cfg = {
    .frame_width = 16,
    .frame_height = 16,
    .fg_color = PixelColor::WHITE,
    .bg_color = PixelColor::BLACK};

/**
 * @brief Example of main program of a SquareLED widget.
 *
 */
int main()
{
    /// main steps:
    pr_D4.hi();
    stdio_init_all();
    ///  1- create I2C bus hw peripheral and display
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);
    /// 2- create test_common_model  as displayed object for blinking_led square_led
    MySquareLedModel my_model = MySquareLedModel();
    /// 3- create square_led as test_square_led_widget
    MyWidgetSquareLed square_led = MyWidgetSquareLed(&my_model, &display, square_led_cfg, 60, 32);
    /// 4- create a switchbutton
    MySwitchButton central_switch = MySwitchButton(CENTRAL_SWITCH_GPIO, cfg_central_switch);
    central_switch.update_current_controlled_object(&my_model);

    /// 5- set led_is_blinking period of the square_led
    square_led.set_blink_us(500000);

    /// 6- clean up the screen and draw the border of the screen
    display.clear_full_screen();

    pr_D4.lo();

    while (true)
    /// 7- start infinite loop

    {
        pr_D5.hi();
        /// - get UI switch button event and process it.
        UIControlEvent event = ((MySwitchButton *)my_model.get_current_controller())->process_sample_event();
        my_model.process_control_event(event);
        /**
         * NOTICE:There is a simpler way to get event. We can also forget UIController and use directly SwitchButton in
         * the following way:
         * \code
         * ControlEvent event = central_switch.process_sample_event();
         * test_common_model.process_control_event(event);
         * \endcode
         *  This avoid the burden of casting UIController and can be used when there is not special need to have UIModelObject aware about wich is displaying its data.
         *
         */

        /// - refresh the widget
        square_led.draw_refresh();
        pr_D5.lo();
        /// - sleep for 20ms
        sleep_ms(20);
    }

    return 0;
}