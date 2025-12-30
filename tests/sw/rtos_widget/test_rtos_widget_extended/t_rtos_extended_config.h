/**
 * @file t_rtos_extended_config.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include "device/SSD1306/ssd1306.h"
#include "device/ST7735/st7735.h"
#include "device/rotary_encoder/rtos_rotary_encoder.h"

//-------------main
#define REFRESH_PERIOD_ms 100
#define DEGREE \xF8
#define ANGLE_INCREMENT 1
#define DUMMY_GPIO_FOR_PERIODIC_EVOLUTION 100
//-------------encoder
#define GLOBAL_TIMEOUT_DELAY_ms 5000
#define SW_TIMEOUT_DELAY_ms 1000
#define UI_MANAGER_TIMEOUT_DELAY_ms GLOBAL_TIMEOUT_DELAY_ms - SW_TIMEOUT_DELAY_ms
#define CENTRAL_SWITCH_GPIO 18
#define ENCODER_CLK_GPIO 19
#define ENCODER_DT_GPIO 20
//--------------display
#define SSD1306_CANVAS_FORMAT CanvasFormat::MONO_VLSB
#define ST7735_GRAPHICS_CANVAS_FORMAT CanvasFormat::RGB565_16b
#define ST7735_TEXT_CANVAS_FORMAT CanvasFormat::RGB565_16b

// #define ST7735_128x128
#define ST7735_128x160

#ifdef ST7735_128x128
#define DEVICE_DISPLAY_TYPE ST7735DisplayType::ST7735_144_128_RGB_128_GREENTAB
#define DEVICE_DISPLAY_ROTATION ST7735Rotation::_90
#define DEVICE_DISPLAY_HEIGHT 128
#endif
#ifdef ST7735_128x160
#define DEVICE_DISPLAY_TYPE ST7735DisplayType::ST7735_177_160_RGB_128_GREENTAB
#define DEVICE_DISPLAY_ROTATION ST7735Rotation::_180
#define DEVICE_DISPLAY_HEIGHT 160
#endif

void i2c_irq_handler();
// ######################### SSD1306 setup ########################################
inline struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE,
    .i2c_tx_master_handler = i2c_irq_handler};

inline struct_ConfigSSD1306 cfg_left_screen{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

inline struct_ConfigSSD1306 cfg_right_screen{
    .i2c_address = 0x3D,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

inline struct_ConfigTextWidget SSD1306_title_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .font = font_12x16,
    .auto_next_char = true};

inline struct_ConfigTextWidget SSD1306_values_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 68,
    .widget_anchor_y = 0,
    .font = font_12x16};

inline struct_ConfigGraphicWidget SSD1306_graph_config{
    .canvas_width_pixel = 128,
    .canvas_height_pixel = 56,
    .canvas_foreground_color = ColorIndex::WHITE,
    .canvas_background_color = ColorIndex::BLACK,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};

// ######################### ST7735 setup ########################################
inline struct_ConfigMasterSPI cfg_spi = {
    .spi = spi1,
    .sck_pin = 10,
    .tx_pin = 11,
    .rx_pin = 12,
    .cs_pin = 13,
    .baud_rate_Hz = 10 * 1000 * 1000};

inline struct_ConfigST7735 cfg_st7735{
    .display_type = DEVICE_DISPLAY_TYPE,
    .backlight_pin = 5,
    .hw_reset_pin = 15,
    .dc_pin = 14,
    .rotation = DEVICE_DISPLAY_ROTATION};

inline struct_ConfigTextWidget ST7735_title_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 0,
    .widget_anchor_y = 64,
    .font = font_12x16,
    .auto_next_char = true};

inline struct_ConfigTextWidget ST7735_values_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 68,
    .widget_anchor_y = 64,
    .font = font_12x16};

inline struct_ConfigGraphicWidget ST7735_graph_config{
    .canvas_width_pixel = 128,
    .canvas_height_pixel = 56,
    .canvas_foreground_color = ColorIndex::YELLOW,
    .canvas_background_color = ColorIndex::RED,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};

