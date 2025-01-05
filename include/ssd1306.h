#if !defined(SSD1306_H)
#define SSD1306_H

#include "ssd1306/commands_SSD1306.h"
#include "pico/stdlib.h"
#include "hw_i2c.h"
#include "framebuffer.h"
// #include "ui_core.h"

// Time_frame_interval
#define _2_FRAMES 0b111
#define _3_FRAMES 0b100
#define _4_FRAMES 0b101
#define _5_FRAMES 0b000
#define _25_FRAMES 0b110
#define _64_FRAMES 0b001
#define _128_FRAMES 0b010
#define _256_FRAMES 0b011

// addressing mode
#define HORIZONTAL_ADDRESSING_MODE 0
#define VERTICAL_ADDRESSING_MODE 1
#define PAGE_ADDRESSING_MODE 2

/**
 * @brief configuration data for SSD1306 OLED display.
 * refer to datasheet for more details.
 */
struct struct_ConfigSSD1306

{
    /// @brief the i2c address, usually 0x3c
    uint8_t i2c_address = 0x3C;
    /// @brief multiplex ratio
    uint8_t mux_ratio_value = 64;
    uint8_t vertical_offset = 0;
    uint8_t GDDRAM_start_line = 0;
    bool scan_SEG_inverse_direction = false;
    bool scan_COM_inverse_direction = false;
    bool sequential_COM = false;
    bool enable_COM_L_R_remap = false;
    uint8_t contrast = 127;
    uint8_t frequency_divider = 1;
    uint8_t frequency_factor = 0;
};

/**
 * @brief configuration data for SSD1306 OLED display scrolling feature.
 * refer to datasheet for more details.
 *
 */
struct struct_ConfigScrollSSD1306
{
    bool scroll_H_to_right = true;           // if true SSD1306_SET_R_HORIZ_SCROLL else SSD1306_SET_L_HORIZ_SCROLL
    bool scroll_V_and_H_to_right = true;     // if true SSD1306_SET_VERTICAL_R_HORIZ_SCROLL else SSD1306_SET_VERTICAL_L_HORIZ_SCROLL
    uint8_t scroll_H_start_page = 0;         // 0 <= value <= 7
    uint8_t time_frame_interval = _2_FRAMES; // 0 <= value <= 7
    uint8_t scroll_H_end_page = 7;           // 0 <= value <= 7
    uint8_t vertical_scrolling_offset = 5;   // 0 <= value <= 63
};

/**
 * @brief data used to compute the render area position in the display framebuffer reference, including the size of the required buffer.
 *
 */
struct struct_RenderArea
{
    uint8_t start_col{0};
    uint8_t end_col{SSD1306_WIDTH - 1};
    uint8_t start_page{0};
    uint8_t end_page{SSD1306_NUM_PAGES - 1};
    size_t width{SSD1306_WIDTH};
    size_t height{SSD1306_HEIGHT};
    size_t buflen{SSD1306_BUF_LEN};
};

/**
 * @brief SSD1306 OLED display device driver
 * 
 */
class SSD1306 : public Framebuffer
{
private:
    HW_I2C_Master *i2c_master;
    struct_ConfigSSD1306 device_config;

    void init();
    void init_MUX_ratio(uint8_t value);
    void init_display_vertical_shift(uint8_t value);
    void init_RAM_start_line(uint8_t value);
    void init_SEG_scan_inverse_direction(bool inverse);
    void init_COM_scan_inverse_direction(bool inverse);
    void init_COM_cfg(bool sequentialCOM, bool enableCOMLRremap);
    void init_clock_frequency(uint8_t divide_ratio, uint8_t frequency_factor);
    void init_charge_pump_enabled(bool enabled);
    void send_cmd(uint8_t cmd);
    void send_cmd_list(uint8_t *cmd_list, int num);
    void send_buf(uint8_t buf[], size_t buflen);

public:
    /**
     * @brief Construct a new SSD1306 object
     *
     * @param master the I2C master interface driver HW_I2C_Master
     * @param device_config the configuration according to struct_ConfigSSD1306
     */
    SSD1306(HW_I2C_Master *master, struct_ConfigSSD1306 device_config);
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
     * @brief the SSD1306 implementation of the show function. Used when we need to show the full device area
     *
     */
    void show();
    /**
     * @brief  the SSD1306 implementation of the show function. Used when we need to show a framebuffer in a given render area
     *
     * @param frame the framebuffer we want to show on the display
     * @param anchor_x where we want to position (x) the frambuffer
     * @param anchor_y where we want to position (y) the frambuffer
     */
    void show(Framebuffer *frame, uint8_t anchor_x, uint8_t anchor_y);
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
     * @param pattern
     * @param area
     */
    void fill_pattern_and_show_GDDRAM(uint8_t pattern, struct_RenderArea area);
    /**
     * @brief write 0x00 into the device framebuffer
     *
     */
    void clear_full_screen();
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
     * 
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

#endif // SSD1306_H
