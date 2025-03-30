/**
 * @file st7735.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
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

// ---------- physical screen size ----------
/// @brief the height in pixel of ST7735
#define ST7735_HEIGHT 128 // can be 160
/// @brief the width in pixel of ST7735
#define ST7735_WIDTH 128 // can be 160

enum class ST7735Rotation{
    _0,
    _90,
    _180,
    _270
};


struct struct_ConfigST7735
{
    size_t device_width = ST7735_WIDTH;
    size_t device_height = ST7735_HEIGHT;
    uint dc_pin;
    uint backlight_pin;
    ST7735Rotation rotation = ST7735Rotation::_0;
    uint hw_reset_pin;

};
struct struct_ConfigScrollST7735
{
};
struct struct_RenderArea
{
};

class ST7735 : public GraphicDisplayDevice
{
private:
    HW_SPI_Master *spi;
    struct_ConfigMasterSPI device_config;
    uint dc_pin;
    uint backlight_pin;
    uint hw_reset_pin;
    void command_pin_enable(bool enable);
    void hardware_reset();
    void init_pins();
    void init_wakeup();
    void init_frame_rate_control();
    void init_inversion_control();
    void init_power_control();
    void init_rotation_and_color();
    void init_column_row_address();
    void init_gamma();
    void init_normal_display_on();
    void init_green_tab();
    void send_cmd(uint8_t cmd);
    void send_cmd_list(uint8_t cmd, size_t len);
    void send_buffer(uint8_t *buffer, size_t buffer_len);
    void set_backlight(bool on);
    void soft_reset();

public:
    ST7735(HW_SPI_Master *spi, struct_ConfigST7735 device_config);
    ~ST7735();
    void display_on(bool on);
    void check_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg);
    void show(struct_PixelFrame *pixel_frame, const uint8_t anchor_x, const uint8_t anchor_y);
    void clear_pixel_buffer(struct_PixelFrame *pixel_frame);
    void create_pixel_buffer(struct_PixelFrame *pixel_frame);
    void pixel(struct_PixelFrame *pixel_frame,
               const int x, const int y,
               const PixelColor color = PixelColor::WHITE);
    void draw_char_into_pixel(struct_PixelFrame *pixel_frame,
                              const struct_ConfigTextWidget text_config,
                              const char character,
                              const uint8_t anchor_x, const uint8_t anchor_y);
};
