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

#include "ssd1306.h"
#include "switch_button.h"
#include "test_tuning_dial/t_tuning_dial_model.cpp"

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
/// @brief define SSD1306 frequency_display config
struct_ConfigSSD1306 cfg_frequency_screen{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};
struct_ConfigSSD1306 cfg_volume_screen{
    .i2c_address = 0x3D,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

struct_ConfigTextFramebuffer fm_text_cnf{
    .number_of_column = 10,
    .number_of_line = 1,
    .font = font_12x16};

///  1- create I2C bus hw peripheral and frequency_display
HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
SSD1306 frequency_display = SSD1306(&master, cfg_frequency_screen);
SSD1306 volume_display = SSD1306(&master, cfg_volume_screen);

FMFrequencyTuningModel my_FM_frequency = FMFrequencyTuningModel(1, true);
FMVolumeModel my_FM_volume = FMVolumeModel(1, true);

TextWidget my_FM_frequency_widget = TextWidget(&frequency_display,
                                               fm_text_cnf,
                                               &my_FM_frequency,
                                               0, 0,
                                               true);
TextWidget my_FM_volume_widget = TextWidget(&volume_display,
                                            fm_text_cnf,
                                            &my_FM_volume,
                                            0, 0,
                                            true);
                                           

int main()
{

    frequency_display.clear_device_screen_buffer();
    volume_display.clear_device_screen_buffer();
    my_FM_frequency.set_clipped_value(my_FM_frequency.get_min_value());
    my_FM_volume.set_clipped_value(my_FM_volume.get_min_value());

    while (true)
    {
        my_FM_frequency.increment_value();
        sprintf(my_FM_frequency_widget.text_buffer, "%5.1f MHz\n", (float)my_FM_frequency.get_value() / 10);
        my_FM_frequency_widget.draw();
        
        my_FM_volume.increment_value();
        sprintf(my_FM_volume_widget.text_buffer, "%*d dB\n", 3, my_FM_volume.get_value());
        my_FM_volume_widget.draw();
        
        sleep_ms(100);
    }

    return 0;
}
