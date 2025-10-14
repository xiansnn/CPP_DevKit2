/**
 * @file ssd1306.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "commands_SSD1306.h"
#include "pico/stdlib.h"
#include "hw/i2c/rtos_hw_i2c.h"
#include "sw/display_device/display_device.h"


// Time_frame_interval
/// @brief refer to SSD1306 data sheet
#define _2_FRAMES 0b111
/// @brief refer to SSD1306 data sheet
#define _3_FRAMES 0b100
/// @brief refer to SSD1306 data sheet
#define _4_FRAMES 0b101
/// @brief refer to SSD1306 data sheet
#define _5_FRAMES 0b000
/// @brief refer to SSD1306 data sheet
#define _25_FRAMES 0b110
/// @brief refer to SSD1306 data sheet
#define _64_FRAMES 0b001
/// @brief refer to SSD1306 data sheet
#define _128_FRAMES 0b010
/// @brief refer to SSD1306 data sheet
#define _256_FRAMES 0b011

// addressing mode
/// @brief refer to SSD1306 data sheet
#define HORIZONTAL_ADDRESSING_MODE 0
/// @brief refer to SSD1306 data sheet
#define VERTICAL_ADDRESSING_MODE 1
/// @brief refer to SSD1306 data sheet
#define PAGE_ADDRESSING_MODE 2

/**
 * @brief configuration data for SSD1306 OLED display.
 * refer to datasheet for more details.
 */
struct struct_ConfigSSD1306

{
    /// @brief the i2c address, usually 0x3C, may be 0x3D
    uint8_t i2c_address = 0x3C;
    /// @brief multiplex ratio
    uint8_t mux_ratio_value = 64;
    /// @brief refer to SSD1306 datasheet for more details.
    uint8_t vertical_offset = 0;
    /// @brief refer to SSD1306 datasheet for more details.
    uint8_t GDDRAM_start_line = 0;
    /// @brief refer to SSD1306 datasheet for more details.
    bool scan_SEG_inverse_direction = false;
    /// @brief refer to SSD1306 datasheet for more details.
    bool scan_COM_inverse_direction = false;
    /// @brief refer to SSD1306 datasheet for more details.
    bool sequential_COM = false;
    /// @brief refer to SSD1306 datasheet for more details.
    bool enable_COM_L_R_remap = false;
    /// @brief refer to SSD1306 datasheet for more details.
    uint8_t contrast = 127;
    /// @brief refer to SSD1306 datasheet for more details.
    uint8_t frequency_divider = 1;
    /// @brief refer to SSD1306 datasheet for more details.
    uint8_t frequency_factor = 0;
};

/**
 * @brief configuration data for SSD1306 OLED display scrolling feature.
 *
 * Refer to SSD1306 datasheet for more details.
 */
struct struct_ConfigScrollSSD1306
{
    /// @brief if true SSD1306_SET_R_HORIZ_SCROLL else SSD1306_SET_L_HORIZ_SCROLL
    bool scroll_H_to_right = true;
    /// @brief if true SSD1306_SET_VERTICAL_R_HORIZ_SCROLL else SSD1306_SET_VERTICAL_L_HORIZ_SCROLL
    bool scroll_V_and_H_to_right = true;
    /// @brief 0 <= value <= 7
    uint8_t scroll_H_start_page = 0;
    /// @brief 0 <= value <= 7
    uint8_t time_frame_interval = _2_FRAMES;
    /// @brief 0 <= value <= 7
    uint8_t scroll_H_end_page = 7;
    /// @brief 0 <= value <= 63
    uint8_t vertical_scrolling_offset = 5;
};

/**
 * @brief data used to compute the render area position in the display framebuffer reference, including the size of the required buffer.
 *
 */
struct struct_RenderArea
{
    /// @brief the pixel x where to start copy of the graphic framebuffer
    uint8_t start_col{0};
    /// @brief the pixel x where to end copy of the graphic framebuffer
    uint8_t end_col{SSD1306_WIDTH - 1};
    /// @brief the page number where to start copy of the graphic framebuffer
    uint8_t start_page{0};
    /// @brief the page number where to end copy of the graphic framebuffer
    uint8_t end_page{SSD1306_NUM_PAGES - 1};
    /// @brief the width of the copy area
    size_t width{SSD1306_WIDTH};
    /// @brief the eight of the copy area
    size_t height{SSD1306_HEIGHT};
    /// @brief the size of the graphic buffer
    size_t buflen{SSD1306_BUF_LEN};
};

/**
 * @brief SSD1306 128x64 pixel OLED display device driver with I2C interface
 * \ingroup view
 */
class SSD1306 : public GraphicDisplayDevice
{
protected:
    /// @brief the I2C master that control the SSD1306 display
    HW_I2C_Master *i2c_master;
    /// @brief the SSD1306 device configuration
    struct_ConfigSSD1306 device_config;

    /// @brief the master init function that calls all others
    void init();
    /// @brief refer to SSD1306 data sheet for more details
    /// @param value refer to SSD1306 data shhet for more details
    void init_MUX_ratio(uint8_t value);
    /// @brief refer to SSD1306 data sheet for more details
    /// @param value refer to SSD1306 data sheet for more details
    void init_display_vertical_shift(uint8_t value);
    /// @brief refer to SSD1306 data sheet for more details
    /// @param value refer to SSD1306 data sheet for more details
    void init_RAM_start_line(uint8_t value);
    /// @brief refer to SSD1306 data sheet for more details
    /// @param inverse refer to SSD1306 data sheet for more details
    void init_SEG_scan_inverse_direction(bool inverse);
    /// @brief refer to SSD1306 data sheet for more details
    /// @param inverse refer to SSD1306 data sheet for more details
    void init_COM_scan_inverse_direction(bool inverse);
    /// @brief refer to SSD1306 data sheet for more details
    /// @param sequentialCOM refer to SSD1306 data sheet for more details
    /// @param enableCOMLRremap refer to SSD1306 data sheet for more details
    void init_COM_cfg(bool sequentialCOM, bool enableCOMLRremap);
    /// @brief refer to SSD1306 data sheet for more details
    /// @param divide_ratio refer to SSD1306 data sheet for more details
    /// @param frequency_factor refer to SSD1306 data sheet for more details
    void init_clock_frequency(uint8_t divide_ratio, uint8_t frequency_factor);
    /// @brief refer to SSD1306 data sheet for more details
    /// @param enabled refer to SSD1306 data sheet for more details
    void init_charge_pump_enabled(bool enabled);
    /// @brief refer to SSD1306 data sheet for more details
    /// @param cmd refer to SSD1306 data sheet for more details
    void send_cmd(uint8_t cmd);
    /// @brief refer to SSD1306 data sheet for more details
    /// @param cmd_list refer to SSD1306 data sheet for more details
    /// @param num refer to SSD1306 data sheet for more details
    void send_cmd_list(uint8_t *cmd_list, int num);
    /// @brief refer to SSD1306 data sheet for more details
    /// @param buf refer to SSD1306 data sheet for more details
    /// @param buflen refer to SSD1306 data sheet for more details
    virtual void send_buf(uint8_t buf[], size_t buflen);

public:

    void check_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg);

    /**
     * @brief
     *
     * @param master the I2C master interface driver HW_I2C_Master
     * @param device_config the configuration according to struct_ConfigSSD1306
     */
    SSD1306(HW_I2C_Master *master, struct_ConfigSSD1306 device_config);

    virtual ~SSD1306();
    /**
     * @brief A static member function that converts the area we want to display into device specific parameters.
     *
     * @param start_col
     * @param end_col
     * @param start_line
     * @param end_line
     * @return struct_RenderArea
     */
    static struct_RenderArea compute_render_area(uint8_t start_col, uint8_t end_col, uint8_t start_line, uint8_t end_line);

    /**
     * @brief
     *
     * @param canvas a pointer to the Canvas that contains the buffer to be displayed
     * @param anchor_x where we want to position (x) the frambuffer
     * @param anchor_y where we want to position (y) the frambuffer
     */
    void show(Canvas *canvas, const uint8_t anchor_x, const uint8_t anchor_y);

    /**
     * @brief  when we need to show a render area with a given framebuffer
     *
     * @param data_buffer
     * @param screen_area
     * @param addressing_mode
     */
    void show_render_area(uint8_t *data_buffer, struct_RenderArea screen_area, uint8_t addressing_mode = HORIZONTAL_ADDRESSING_MODE);
    /**
     * @brief fill a pattern in the device framebuffer. this make it visible as soon as the device transfer the framebuffer to the pixels.
     * The pattern is a vertical byte representing 8 vertical pixels (refer to MONO_VLSB framebuffer format)
     * \bug// FIXME : PAGE_ADDRESSING_MODE seems misbehave depending on what was executed before
     * @param pattern
     * @param area
     */
    void fill_pattern_and_show_GDDRAM(uint8_t pattern, struct_RenderArea area);
    /**
     * @brief write 0x00 directly into the device framebuffer.
     *  Uses fill_pattern_and_show_GDDRAM command.
     */
    void clear_device_screen_buffer();
    /**
     * @brief Set the contrast object. refer to datasheet of SSD1306 device.
     *
     * @param value
     */
    void set_contrast(uint8_t value);
    /**
     * @brief Set the display from RAM object
     *
     */
    void set_display_from_RAM();
    /**
     * @brief Set the all pixel ON
     */
    void set_all_pixel_ON();
    /**
     * @brief Set the inverse color object
     *
     * @param inverse
     */
    void set_inverse_color(bool inverse);
    /**
     * @brief Set the display OFF object
     *
     */
    void set_display_OFF();
    /**
     * @brief Set the display ON object
     *
     */
    void set_display_ON();
    /**
     * @brief
     *
     * @param on
     * @param scroll_data
     */
    void horizontal_scroll(bool on, struct_ConfigScrollSSD1306 scroll_data);
    /**
     * @brief
     *
     * @param on
     * @param scroll_data
     */
    void vertical_scroll(bool on, struct_ConfigScrollSSD1306 scroll_data);
};

class rtos_SSD1306 : public SSD1306
{
private:
    /* data */
public:
    rtos_SSD1306(rtos_HW_I2C_Master *master, struct_ConfigSSD1306 device_config);
    ~rtos_SSD1306();

    /// @brief refer to SSD1306 data sheet for more details
    /// @param buf refer to SSD1306 data sheet for more details
    /// @param buflen refer to SSD1306 data sheet for more details
    void send_buf(uint8_t buf[], size_t buflen);

};

