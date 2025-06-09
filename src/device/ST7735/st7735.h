/**
 * @file st7735.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief inspired from various code source, including https://github.com/adafruit/Adafruit-ST7735-Library
 * @version 0.1
 * @date 2025-03-29
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "pico/stdlib.h"
#include "commands_ST7735.h"
#include "hw/spi/hw_spi.h"
#include "sw/display_device/display_device.h"

/// @brief level defined as command on the data/Command wire
#define DCX_COMMAND 0
/// @brief level defined as data on the data/Command wire
#define DCX_DATA 1
/// @brief hardware column offset between the TFT panel and the IC ST7735
#define ST7735_144_128x128_column_offset 2
/// @brief hardware row offset between the TFT panel and the IC ST7735 for rotation = 0°, 90°
#define ST7735_144_128x128_row_offset_0_90 1
/// @brief hardware row offset between the TFT panel and the IC ST7735 for rotation = 180°, 270°
#define ST7735_144_128x128_row_offset_180_270 3
/// @brief hardware column offset between the TFT panel and the IC ST7735
#define ST7735_177_160x128_column_offset 0//2
/// @brief hardware row offset between the TFT panel and the IC ST7735
#define ST7735_177_160x128_row_offset 0//1

/// @brief tag used to ru the correct init code
enum class ST7735DisplayType
{
    /// @brief LCD TFT 1"44 128(RGB)x128 green_tab configuration
    ST7735_144_128_RGB_128_GREENTAB, // 
    /// @brief LCD TFT 1"77 160(RGB)x128 green_tab configuration
    ST7735_177_160_RGB_128_GREENTAB  // 
};

/// @brief code indicates rotation of the display (Portrait, paysage, upside-down)
enum class ST7735Rotation
{
    
    /// @brief no rotation applied
    _0,
    /// @brief rotate 90° clockwise
    _90,
    /// @brief rotate 180° clockwise
    _180,
    /// @brief rotate 270° clockwise 
    _270
};

/// @brief data used to configure the display device
struct struct_ConfigST7735
{
    
    /// @brief indicates which type of TFT panel must be configured
    ST7735DisplayType display_type;
    /// @brief indicates the GPIO connected to the backlight input
    uint backlight_pin;
    /// @brief indicates the GPIO connected to the hardware reset input
    uint hw_reset_pin;
    /// @brief indicates the GPIO connected to the Data/Command_ input
    uint dc_pin;
    /// @brief indicates which rotation will be applied
    ST7735Rotation rotation = ST7735Rotation::_0;
};
struct struct_ConfigScrollST7735
{
};

/**
 * @brief ST7735 driven TFT color display device
 * \ingroup view
 * 
 */
class ST7735 : public GraphicDisplayDevice
{
private:
    HW_SPI_Master *spi;
    uint dc_pin;
    uint backlight_pin;
    uint hw_reset_pin;
    uint8_t ST7735_device_column_offset{0};
    uint8_t ST7735_device_row_offset{0};
    uint8_t TFT_panel_start_x{0};
    uint8_t TFT_panel_start_y{0};
    bool rgb_order;

    void enable_command_pin(bool enable);
    void hardware_reset();
    void init_pins();
    void device_wakeup();
    void config_frame_rate_control();
    void config_inversion_control();
    void config_power_control();
    // void init_column_row_address(struct_ConfigST7735 device_config);
    void config_gamma();
    void set_normal_mode();
    // void init_green_tab();
    void config_device_specific_size_and_offsets(struct_ConfigST7735 device_config);
    void send_cmd(uint8_t cmd);
    void send_cmd_list(uint8_t *cmd, size_t len);
    void send_buffer(uint8_t *buffer, size_t buffer_len);
    void set_backlight(bool on);
    void soft_reset();
    
    void set_RAM_write_addresses(uint8_t start_x, uint8_t start_y, size_t width, size_t height);
    
    public:

    /// @brief Construct a new ST7735 object
    /// @param spi the SPI associated driver
    /// @param device_config the device configuration file
    ST7735(HW_SPI_Master *spi, struct_ConfigST7735 device_config);
    ~ST7735();

    /// @brief set the actual position of the display
    ///  @param device_config the device configuration file
    void set_rotation_and_color(struct_ConfigST7735 device_config);

    /// @brief Set the display ON object
    void set_display_ON();

    /// @brief Set the display OFF object
    void set_display_OFF();

    /// @brief turn the display in sleep mode on/off
    /// @param enable 
    void enable_sleep(bool enable);
    void check_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg);

    /// @brief fill the internal ST7735 screen buffer with the given color.
    /// Default to BLACK, clear the internal buffer
    /// @param color_index 
    void clear_device_screen_buffer(ColorIndex color_index = ColorIndex::BLACK);
    void show(Canvas *canvas, const uint8_t anchor_x, const uint8_t anchor_y);
};
