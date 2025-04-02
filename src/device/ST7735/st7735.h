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

#define DCX_COMMAND 0
#define DCX_DATA 1

enum class ST7735DisplayType
{
    ST7735_144_128_RGB_128_GREENTAB, // LCD TFT 1"44 128(RGB)x128 green_tab configuration
    ST7735_177_160_RGB_128_GREENTAB  // LCD TFT 1"77 160(RGB)x128 green_tab configuration
};

enum class ST7735Rotation
{
    _0,
    _90,
    _180,
    _270
};

struct struct_ConfigST7735
{
    ST7735DisplayType display_type;
    uint backlight_pin;
    uint hw_reset_pin;
    uint dc_pin;
    ST7735Rotation rotation = ST7735Rotation::_0;
};
struct struct_ConfigScrollST7735
{
};
struct struct_RenderArea
{
    /// @brief the pixel x where to start copy of the graphic framebuffer to the device memory
    uint8_t start_col{0};
    /// @brief the pixel y where to end copy of the graphic framebuffer to the device memory
    uint8_t start_row{0};
    /// @brief the width of the rendered area
    size_t width{};
    /// @brief the eight of the rendered area
    size_t height{};
    /// @brief the size of the graphic buffer
    size_t buflen{};
};

class ST7735 : public GraphicDisplayDevice
{
private:
    HW_SPI_Master *spi;
    uint dc_pin;
    uint backlight_pin;
    uint hw_reset_pin;
    uint8_t device_start_column_offset{0};
    uint8_t device_start_row_offset{0};
    uint8_t device_start_x{0};
    uint8_t device_start_y{0};
    bool rgb_order;

    void enable_command_pin(bool enable);
    void hardware_reset();
    void init_pins();
    void device_wakeup();
    void config_frame_rate_control();
    void config_inversion_control();
    void config_power_control();
    void config_rotation_and_color(struct_ConfigST7735 device_config);
    void init_column_row_address(struct_ConfigST7735 device_config);
    void config_gamma();
    void set_normal_display_on();
    // void init_green_tab();
    void config_device_specific_size_and_offsets(struct_ConfigST7735 device_config);
    void send_cmd(uint8_t cmd);
    void send_cmd_list(uint8_t *cmd, size_t len);
    void send_buffer(uint8_t *buffer, size_t buffer_len);
    void set_backlight(bool on);
    void soft_reset();

public:
    ST7735(HW_SPI_Master *spi, struct_ConfigST7735 device_config);
    ~ST7735();
    void enable_display(bool enable);
    void enable_sleep(bool enable);
    void check_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg);
    void show(struct_PixelFrame *pixel_frame, const uint8_t anchor_x, const uint8_t anchor_y);
    void clear_pixel_buffer(struct_PixelFrame *pixel_frame);
    void create_pixel_buffer(struct_PixelFrame *pixel_frame);
    void pixel(struct_PixelFrame *pixel_frame,
               const int x, const int y,
               const ColorIndex color = ColorIndex::WHITE);
    void draw_char_into_pixel(struct_PixelFrame *pixel_frame,
                              const struct_ConfigTextWidget text_config,
                              const char character,
                              const uint8_t anchor_x, const uint8_t anchor_y);
};
