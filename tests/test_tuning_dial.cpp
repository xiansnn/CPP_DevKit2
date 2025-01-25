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
WidgetText my_FM_widget = WidgetText(&my_FM_model,
                                     &display,
                                     fm_text_cnf,
                                     10, 1,
                                     0, 0,
                                     false);

int main()
{

    display.clear_full_screen();

    my_FM_model.set_current_frequency( 87.0);
    while (true)
    {
        my_FM_model.process_control_event();
        sprintf(my_FM_widget.text_buffer, "%5.1f MHz\n", my_FM_model.get_current_frequency());
        my_FM_widget.draw_refresh();
        my_FM_model.increment_frequency(0.5);

        sleep_ms(500);
    }

    return 0;
}
