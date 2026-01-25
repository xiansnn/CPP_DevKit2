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
// #define SHOW_I2C_DISPLAY
// #define SHOW_SPI_FOCUS_INDICATOR

#include "utilities/probe/probe.h"
extern Probe p0;
extern Probe p1;

#if !defined(SHOW_I2C_DISPLAY)
extern Probe p2; // I2C SDA
extern Probe p3; // I2C SCL
#endif 

extern Probe p4;    
extern Probe p5;
extern Probe p6;
extern Probe p7;





#ifdef SHOW_I2C_DISPLAY
#include "device/SSD1306/ssd1306.h"
#endif
#include "device/ST7735/st7735.h"
#include "device/rotary_encoder/rtos_rotary_encoder.h"

//-------------main
#define PERIOD_REFRESH_ms 100
#define CHAR_DEGREE \xF8
#define ANGLE_INCREMENT 1
#define GPIO_FOR_PERIODIC_EVOLUTION 100
//-------------encoder
#define TIMEOUT_GLOBAL_DELAY_ms 5000
#define TIMEOUT_SWITCH_DELAY_ms 1000
#define TIMEOUT_UI_MANAGER_DELAY_ms TIMEOUT_GLOBAL_DELAY_ms - TIMEOUT_SWITCH_DELAY_ms
#define GPIO_CENTRAL_SWITCH 18
#define GPIO_ENCODER_CLK 19
#define GPIO_ENCODER_DT 20
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


