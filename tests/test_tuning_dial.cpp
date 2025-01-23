/**
 * @file test_tuning_dial.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "widget_text.h"
#include "ssd1306.h"
#include "switch_button.h"
#include "test_tuning_dial/t_tuning_dial_model.cpp"

/// @brief define central switch config
struct_SwitchButtonConfig cfg_central_switch{
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

struct_FramebufferText fm_text_cnf{
    .font = font_8x8};

///  1- create I2C bus hw peripheral and display
HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
SSD1306 display = SSD1306(&master, cfg_ssd1306);

FMFrequencyTuning my_FM_model = FMFrequencyTuning();
WidgetTextWithFocus my_FM_widget = WidgetTextWithFocus(&my_FM_model, &display, 100, 10, 0, 0, false);

int main()
{
    stdio_init_all();

    my_FM_widget.init_text_buffer(fm_text_cnf);

    sprintf(my_FM_widget.text_buffer, "%c     %5.1f MHz", my_FM_widget.convert_status_to_char(), my_FM_model.current_frequency / 10);
    my_FM_widget.print_text();
    display.show();



    return 0;
}
