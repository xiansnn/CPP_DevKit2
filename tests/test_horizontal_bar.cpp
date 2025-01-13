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

#include "ssd1306.h"
#include "probe.h"

#include "widget_horizontal_bar.h"
#include "ui_core.h"

// #include "test_square_led/t_switch_button_controller.cpp"
// #include "test_square_led/t_square_led_widget.cpp"

/// @brief ########## Debug/Observer Probe for logic analyser section ##########
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D1 = Probe(1);

/// @brief ########## configuration section ##########

#define MAX_VALUE 3
#define MIN_VALUE -20

/// @brief define central switch config
// struct_SwitchButtonConfig cfg_central_switch{
//     .debounce_delay_us = 5000,
//     .long_release_delay_us = 1000000,
//     .long_push_delay_us = 1000000,
//     .active_lo = true};
// #define CENTRAL_SWITCH_GPIO 6

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

class MyModel : public UIControlledIncrementalValue
{
private:
    /* data */
public:
    MyModel(int _min_value, int _max_value, bool _is_wrappable, int _increment);
    ~MyModel();
    void process_control_event(UIControlEvent _event);
};

MyModel::MyModel(int _min_value, int _max_value, bool _is_wrappable, int _increment)
    : UIControlledIncrementalValue(_min_value, _max_value, _is_wrappable, _increment)
{
}

MyModel::~MyModel()
{
}

void MyModel::process_control_event(UIControlEvent _event)
{
    switch (_event)
    {
    case UIControlEvent::INCREMENT:
        this->increment_value();
        break;
    case UIControlEvent::DECREMENT:
        this->decrement_value();
        break;
    default:
        break;
    }
}

int main()
{
    pr_D4.hi();
    stdio_init_all();

    MyModel my_model = MyModel(MIN_VALUE, MAX_VALUE, true, 1);

    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);
    WidgetHorizontalBar my_horizontal_bar = WidgetHorizontalBar(&my_model, &display, MAX_VALUE, MIN_VALUE, 100, 8, 20, 8);
    /// 2- create test_common_model  as displayed object for blinking_led square_led
    // MySquareLedModel my_model = MySquareLedModel();
    /// 3- create square_led as test_square_led_widget
    // MyWidgetSquareLed square_led = MyWidgetSquareLed(&my_model, &display, 16, 16, 60, 32);
    /// 4- create a switchbutton
    // MySwitchButton central_switch = MySwitchButton(CENTRAL_SWITCH_GPIO, cfg_central_switch);
    // central_switch.update_current_controlled_object(&my_model);

    /// 5- set led_is_blinking period of the square_led
    // square_led.set_blink_us(500000);

    /// 6- clean up the screen and draw the border of the screen
    display.clear_pixel_buffer();
    display.rect(0, 0, 128, 64);
    display.show();
    pr_D4.lo();

    while (true)
    /// 7- start infinite loop

    {
        pr_D5.hi();

        my_model.increment_value();
        my_horizontal_bar.set_value(my_model.get_value());

        /// - refresh the widget
        my_horizontal_bar.draw_refresh();
        pr_D5.lo();
        /// - sleep for 20ms
        sleep_ms(1000);
    }

    return 0;
}