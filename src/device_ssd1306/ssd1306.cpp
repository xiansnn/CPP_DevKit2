#include "ssd1306.h"
#include <string.h>

void SSD1306::send_cmd(uint8_t cmd)
{
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command
    this->i2c_master->single_byte_write(this->device_config.i2c_address, I2C_CMD_FLAG, cmd);
}

void SSD1306::send_cmd_list(uint8_t *cmd_list, int cmd_list_size)
{
    for (int i = 0; i < cmd_list_size; i++)
        this->send_cmd(cmd_list[i]);
}

void SSD1306::send_buf(uint8_t buffer[], size_t buffer_size)
{
    this->i2c_master->burst_byte_write(this->device_config.i2c_address, I2C_DATA_FLAG, buffer, buffer_size);
}

SSD1306::SSD1306(HW_I2C_Master *master, struct_ConfigSSD1306 init_config)
    : UIDisplayDevice(SSD1306_WIDTH, SSD1306_HEIGHT, FramebufferFormat::MONO_VLSB)
{
    this->i2c_master = master;
    this->device_config = init_config;
    this->init();
}

struct_RenderArea SSD1306::compute_render_area(uint8_t start_col, uint8_t end_col, uint8_t start_line, uint8_t end_line)
{
    struct_RenderArea area;
    area.start_col = start_col;
    area.end_col = end_col;
    area.start_page = start_line / SSD1306_PAGE_HEIGHT;
    area.end_page = end_line / SSD1306_PAGE_HEIGHT;
    area.width = end_col - start_col + 1;
    area.height = end_line - start_line + 1;
    area.buflen = (area.width) * (area.end_page - area.start_page + 1);
    return area;
}

void SSD1306::set_display_from_RAM()
{
    this->send_cmd(SSD1306_SET_RAM_DISPLAY);
}

void SSD1306::set_all_pixel_ON()
{
    this->send_cmd(SSD1306_SET_DISPLAY_ON);
}

void SSD1306::set_inverse_color(bool inverse)
{
    if (inverse)
        this->send_cmd(SSD1306_SET_INV_DISP);
    else
        this->send_cmd(SSD1306_SET_NORM_DISP);
}

void SSD1306::set_display_OFF()
{
    this->send_cmd(SSD1306_SET_DISPLAY_SLEEP);
}

void SSD1306::set_display_ON()
{
    this->send_cmd(SSD1306_SET_DISPLAY_NORMAL_ON);
}

void SSD1306::fill_pattern_and_show_GDDRAM(uint8_t pattern, struct_RenderArea area)
{
    uint8_t image[SSD1306_BUF_LEN];
    memset(image, pattern, area.buflen);
    this->show_render_area(image, area);
}

void SSD1306::clear_full_screen()
{
    this->clear_pixel_buffer();
    this->show();
}

void SSD1306::init()
{
    this->set_display_OFF();
    this->init_MUX_ratio(device_config.mux_ratio_value);
    this->init_display_vertical_shift(device_config.vertical_offset);
    this->init_RAM_start_line(device_config.GDDRAM_start_line);
    this->init_SEG_scan_inverse_direction(device_config.scan_SEG_inverse_direction);
    this->init_COM_scan_inverse_direction(device_config.scan_COM_inverse_direction);
    this->init_COM_cfg(device_config.sequential_COM, device_config.enable_COM_L_R_remap);
    this->set_contrast(device_config.contrast);
    this->set_display_from_RAM();
    this->set_inverse_color(false);
    this->init_clock_frequency(device_config.frequency_divider, device_config.frequency_factor);
    this->init_charge_pump_enabled(true);
    this->set_display_ON();

    uint8_t cmds[] = {
        SSD1306_SET_MEM_MODE,   // set memory address mode 0 = horizontal, 1 = vertical, 2 = page
        0x00,                   // horizontal addressing mode
        SSD1306_SET_SCROLL_OFF, // deactivate horizontal scrolling if set. This is necessary as memory writes will corrupt if scrolling was enabled
    };

    this->send_cmd_list(cmds, count_of(cmds));
}

void SSD1306::init_MUX_ratio(uint8_t value)
{
    assert((value >= 1) & (value <= 64));
    this->send_cmd(SSD1306_SET_MUX_RATIO);
    this->send_cmd(value - 1);
}

void SSD1306::show_render_area(uint8_t *data_buffer, const struct_RenderArea screen_area, const uint8_t addressing_mode)
{
    assert((addressing_mode >= 0) & (addressing_mode <= 2));
    this->send_cmd(SSD1306_SET_MEM_MODE);
    this->send_cmd(addressing_mode);
    if (addressing_mode == PAGE_ADDRESSING_MODE)
    {
        uint8_t page_start_address = 0xB0 | screen_area.start_page;
        this->send_cmd(page_start_address);
        uint8_t column_start_LO_address = 0x0F & screen_area.start_col;
        this->send_cmd(column_start_LO_address);
        uint8_t column_start_HI_address = (((0xF0) & screen_area.start_col) >> 4) | 0x10;
        this->send_cmd(column_start_HI_address);
        this->send_buf(data_buffer, screen_area.buflen);
    }
    else
    {
        this->send_cmd(SSD1306_SET_COL_ADDR);
        this->send_cmd(screen_area.start_col);
        this->send_cmd(screen_area.end_col);
        this->send_cmd(SSD1306_SET_PAGE_ADDR);
        this->send_cmd(screen_area.start_page);
        this->send_cmd(screen_area.end_page);
        this->send_buf(data_buffer, screen_area.buflen);
    }
}

void SSD1306::show()
{
    this->show_render_area(this->pixel_buffer, this->compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1));
}

void SSD1306::show(Framebuffer *frame, uint8_t anchor_x, uint8_t anchor_y)
{
    uint8_t end_col = anchor_x + frame->frame_width - 1;
    uint8_t end_line = anchor_y + frame->frame_height - 1;

    assert(end_col <= SSD1306_WIDTH - 1);
    assert(end_line <= SSD1306_HEIGHT - 1);

    this->show_render_area(frame->pixel_buffer, this->compute_render_area(anchor_x, end_col, anchor_y, end_line));
}

void SSD1306::init_display_vertical_shift(uint8_t value)
{
    assert((value >= 0) & (value <= 63));
    this->send_cmd(SSD1306_SET_DISP_OFFSET);
    this->send_cmd(value);
}

void SSD1306::init_RAM_start_line(uint8_t value)
{
    assert((value >= 0) & (value <= 63));
    this->send_cmd(SSD1306_SET_DISP_START_LINE);
    this->send_cmd(value);
}

void SSD1306::init_SEG_scan_inverse_direction(bool inverse)
{
    if (inverse == true)
        this->send_cmd(SSD1306_SET_SEG_REMAP);
    else
        this->send_cmd(SSD1306_SET_SEG_NO_REMAP);
}

void SSD1306::init_COM_scan_inverse_direction(bool inverse)
{
    if (inverse == true)
        this->send_cmd(SSD1306_SET_COM_OUT_DIR_REMAP);
    else
        this->send_cmd(SSD1306_SET_COM_OUT_DIR);
}

void SSD1306::init_COM_cfg(bool sequential_COM, bool enable_COM_L_R_remap)
{
    uint8_t value = 0x02;
    this->send_cmd(SSD1306_SET_COM_PIN_CFG);
    if (!sequential_COM)
        value = value | 0x10;
    if (enable_COM_L_R_remap)
        value = value | 0x20;
    this->send_cmd(value);
}

void SSD1306::set_contrast(uint8_t value)
{
    assert((value >= 0) & (value <= 255));
    this->send_cmd(SSD1306_SET_CONTRAST);
    this->send_cmd(value);
}

void SSD1306::init_clock_frequency(uint8_t divide_ratio, uint8_t frequency_factor)
{
    assert((divide_ratio >= 1) & (divide_ratio <= 16));
    assert((frequency_factor >= 0) & (frequency_factor <= 15));
    uint8_t value = (frequency_factor << 4) | (divide_ratio - 1);
    this->send_cmd(SSD1306_SET_DISP_CLK_DIV);
    this->send_cmd(value);
}

void SSD1306::init_charge_pump_enabled(bool enabled)
{
    this->send_cmd(SSD1306_SET_PRECHARGE);
    this->send_cmd(0x22);
    this->send_cmd(SSD1306_SET_VCOM_DESEL);
    this->send_cmd(0x20);
    uint8_t value;
    this->send_cmd(SSD1306_SET_CHARGE_PUMP);
    if (enabled)
        value = 0x14;
    else
        value = 0x10;
    this->send_cmd(value);
}

void SSD1306::horizontal_scroll(bool on, struct_ConfigScrollSSD1306 scroll_data)
{ // configure horizontal scrolling by 1 column
    uint8_t cmds[8];
    if (scroll_data.scroll_H_to_right)
        cmds[0] = SSD1306_SET_R_HORIZ_SCROLL;
    else
        cmds[0] = SSD1306_SET_L_HORIZ_SCROLL;
    cmds[1] = 0x00;
    cmds[2] = scroll_data.scroll_H_start_page;
    cmds[3] = scroll_data.time_frame_interval;
    cmds[4] = scroll_data.scroll_H_end_page;
    cmds[5] = 0x00;
    cmds[6] = 0xFF;
    if (on)
        cmds[7] = SSD1306_SET_SCROLL_ON;
    else
        cmds[7] = SSD1306_SET_SCROLL_OFF;
    this->send_cmd_list(cmds, count_of(cmds));
}

void SSD1306::vertical_scroll(bool on, struct_ConfigScrollSSD1306 scroll_data)
{
    uint8_t cmds[7];
    if (scroll_data.scroll_H_to_right)
        cmds[0] = SSD1306_SET_VERTICAL_R_HORIZ_SCROLL;
    else
        cmds[0] = SSD1306_SET_VERTICAL_L_HORIZ_SCROLL;
    cmds[1] = 0x00;
    cmds[2] = scroll_data.scroll_H_start_page;
    cmds[3] = scroll_data.time_frame_interval;
    cmds[4] = scroll_data.scroll_H_end_page;
    cmds[5] = scroll_data.vertical_scrolling_offset;
    if (on)
        cmds[6] = SSD1306_SET_SCROLL_ON;
    else
        cmds[6] = SSD1306_SET_SCROLL_OFF;
    this->send_cmd_list(cmds, count_of(cmds));
}

/*

static uint8_t reversed[sizeof(font)] = {0};

static uint8_t reverse(uint8_t b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}
static void FillReversedCache()
{
    // calculate and cache a MONO_VLSB version of fhe font, because it's defined MONO_VMSB (upside down)
    for (size_t i = 0; i < sizeof(font); i++)
        reversed[i] = reverse(font[i]);
}
*/