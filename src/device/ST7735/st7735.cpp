#include "st7735.h"
#include "color/color_palette.h"
#include <string.h>

void ST7735::enable_command_pin(bool enable)
{
    gpio_put(dc_pin, enable ? DCX_COMMAND : DCX_DATA);
}

void ST7735::hardware_reset()
{
    gpio_put(hw_reset_pin, 0);
    sleep_us(20);
    gpio_put(hw_reset_pin, 1);
    sleep_ms(120);
}

void ST7735::init_pins()
{
    gpio_init(dc_pin);
    gpio_set_dir(dc_pin, GPIO_OUT);
    gpio_pull_up(dc_pin);
    gpio_put(dc_pin, 1);

    gpio_init(hw_reset_pin);
    gpio_set_dir(hw_reset_pin, GPIO_OUT);
    gpio_pull_up(hw_reset_pin);
    gpio_put(hw_reset_pin, 1);

    gpio_init(backlight_pin);
    gpio_set_dir(backlight_pin, GPIO_OUT);
}

/**
 * @brief _ wake up_
 * harware_reset
 * SW_reset, sleep 150us
 * SLPOUT, sleep 150us
 */
void ST7735::device_wakeup()
{
    hardware_reset();
    enable_sleep(false);
}

/**
 * @brief _Frame rate control_
 * -Set the frame frequency of the full colors normal mode.
 * - Frame rate=fosc/((param[1] x 2 + 40) x (LINE + param[2] + param[3] +2))
 * -fosc = 850kHz
 * -FPA > 0, BPA > 0
 * FRMCTR1 , [0x01, 0x2C, 0x2D] normal mode fastest refresh, 6 lines front, 3 lines back
 * FRMCTR2 , [0x01, 0x2C, 0x2D] idle mode fastest refresh, 6 lines front, 3 lines back
 * FRMCTR3 , [0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d],  partial mode sleep 10us
 */
void ST7735::config_frame_rate_control()
{
    uint8_t cmd_list[7];
    cmd_list[1] = 0x01;
    cmd_list[2] = 0x2C;
    cmd_list[3] = 0x2D;
    cmd_list[4] = 0x01;
    cmd_list[5] = 0x2C;
    cmd_list[6] = 0x2D;

    cmd_list[0] = ST7735_FRMCTR1;
    send_cmd_list(cmd_list, 4);
    cmd_list[0] = ST7735_FRMCTR2;
    send_cmd_list(cmd_list, 4);
    cmd_list[0] = ST7735_FRMCTR3;
    send_cmd_list(cmd_list, 7);
}

/**
 * @brief  _display inversion control_
 * INVCTR , [0x07] column inversion and no dot inversion for Normal, Idle and partial mode
 */
void ST7735::config_inversion_control()
{
    uint8_t cmd_list[] = {ST7735_INVCTR, 0x07};
    send_cmd_list(cmd_list, 2);
}

/**
 * @brief _power control_
 * PWCTR1 , [0xA2,0x02,0x84]
 * PWCTR2 , [0xC5] #VGH = 14.7V, VGL = -7.35V
 * PWCTR3 , [0x0A, 0x00] opamp current small, boost frequency
 * PWCTR4 , [0x8A, 0x2A] opamp current small, boost frequency
 * PWCTR5 , [0x8A, 0xEE] opamp current small, boost frequency
 * VMCTR1 , [0x0E]
 */
void ST7735::config_power_control()
{
    uint8_t cmd_list[4];
    cmd_list[0] = ST7735_PWCTR1;
    cmd_list[1] = 0xA2; // AVDD = 5V ; GVDD = 4.6V
    cmd_list[2] = 0x02; // GVCL = -4.6V
    cmd_list[3] = 0x84; // Mode = Auto
    send_cmd_list(cmd_list, 4);

    cmd_list[0] = ST7735_PWCTR2;
    cmd_list[1] = 0xC5; // VGH25 = 2.4V ; VGLSEL = -10V ; VGHBT = 3*AVDD-0.5 V
    send_cmd_list(cmd_list, 2);

    cmd_list[0] = ST7735_PWCTR3; // power control in normal mode
    cmd_list[1] = 0x0A;          // amount of current in Opamp = medium low ; amount of current in Opamp = small
    cmd_list[2] = 0x00;          // set up boost circuit
    send_cmd_list(cmd_list, 3);

    cmd_list[0] = ST7735_PWCTR4; // power control in idle mode
    cmd_list[1] = 0x8A;          // amount of current in Opamp = medium low ; amount of current in Opamp = small
    cmd_list[2] = 0x2A;          // set up boost circuit
    send_cmd_list(cmd_list, 3);

    cmd_list[0] = ST7735_PWCTR5; // power control in partial mode
    cmd_list[1] = 0x8A;
    cmd_list[2] = 0xEE;
    send_cmd_list(cmd_list, 3);

    cmd_list[0] = ST7735_VMCTR1; // VCOM voltage
    cmd_list[1] = 0x0E;          // VCOM = -0.775 V
    send_cmd_list(cmd_list, 2);
}

/**
 * @brief  _set rotation and color_
 */
void ST7735::config_rotation_and_color(struct_ConfigST7735 device_config)
{
    uint8_t cmd_list[2]{0};
    cmd_list[0] = ST7735_MADCTL;
    switch (device_config.rotation)
    {
    case ST7735Rotation::_0:
        this->TFT_panel_start_x = this->ST7735_device_column_offset;
        this->TFT_panel_start_y = this->ST7735_device_row_offset;
        break;
    case ST7735Rotation::_90:
        cmd_list[1] = MADCTL_MV | MADCTL_MX;
        this->TFT_panel_start_x = this->ST7735_device_row_offset;
        this->TFT_panel_start_y = this->ST7735_device_column_offset;
        break;
    case ST7735Rotation::_180:
        cmd_list[1] = MADCTL_MX | MADCTL_MY;
        this->TFT_panel_start_x = this->ST7735_device_column_offset;
        this->TFT_panel_start_y = this->ST7735_device_row_offset;
        break;
    case ST7735Rotation::_270:
        cmd_list[1] = MADCTL_MV | MADCTL_MY;
        this->TFT_panel_start_x = this->ST7735_device_row_offset;
        this->TFT_panel_start_y = this->ST7735_device_column_offset;
        break;
    default:
        break;
    }
    if (!rgb_order)
        cmd_list[1] |= MADCTL_BGR;

    send_cmd_list(cmd_list, 2);

    cmd_list[0] = ST7735_COLMOD;
    cmd_list[1] = 0x05; // 16-bit/pixel
    send_cmd_list(cmd_list, 2);
}

/**
 * @brief  _column row address set_
 * CASET , [0x00,0x01,0x00, width-1]
 * RASET , [0x00,0x01,0x00, height-1]
 * we start column 1
 *
 * ST77XX_CASET,   4,              //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,                   //     XSTART = 0
      0x00, 0x7F+0x02,              //     XEND = 127
    ST77XX_RASET,   4,              //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,                   //     XSTART = 0
      0x00, 0x9F+0x01 },            //     XEND = 159
 */
// void ST7735::init_column_row_address(struct_ConfigST7735 device_config)
// {
//     uint8_t cmd_list[5];
//     switch (device_config.display_type)
//     {
//     case ST7735DisplayType::ST7735_144_128_RGB_128_GREENTAB:
//         cmd_list[0] = ST7735_CASET;
//         cmd_list[1] = 0x00;
//         cmd_list[2] = TFT_panel_start_x;
//         cmd_list[3] = 0x00;
//         cmd_list[4] = 0x7F + TFT_panel_start_x;
//         send_cmd_list(cmd_list, 5);
//         cmd_list[0] = ST7735_RASET;
//         cmd_list[1] = 0x00;
//         cmd_list[2] = TFT_panel_start_y;
//         cmd_list[3] = 0x00;
//         cmd_list[4] = 0x7F + TFT_panel_start_y;
//         send_cmd_list(cmd_list, 5);
//         break;
//     case ST7735DisplayType::ST7735_177_160_RGB_128_GREENTAB:
//         cmd_list[0] = ST7735_CASET;
//         cmd_list[1] = 0x00;
//         cmd_list[2] = ST7735_device_column_offset;
//         cmd_list[3] = 0x00;
//         cmd_list[4] = 0x7F + ST7735_device_column_offset;
//         send_cmd_list(cmd_list, 5);
//         cmd_list[0] = ST7735_RASET;
//         cmd_list[1] = 0x00;
//         cmd_list[2] = ST7735_device_row_offset;
//         cmd_list[3] = 0x00;
//         cmd_list[4] = 0x9F + ST7735_device_row_offset;
//         send_cmd_list(cmd_list, 5);

//         break;

//     default:
//         break;
//     }
// }

/**
 * @brief _gamma correction_
 * GMCTRP1 , [0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10]
 * GMCTRN1 , [0x03, 0x1d, 0x07, 0x06, 0x2e, 0x2c, 0x29, 0x2d, 0x2e, 0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10]
 */
void ST7735::config_gamma()
{
    // uint8_t cmd_list[17] = {ST7735_GAMCTRP1, 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10};
    // send_cmd_list(cmd_list, 17);
    // cmd_list[17] = {ST7735_GAMCTRN1, 0x03, 0x1d, 0x07, 0x06, 0x2e, 0x2c, 0x29, 0x2d, 0x2e, 0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10};
    // send_cmd_list(cmd_list, 17);
}

/**
 * @brief  _set display normal ON
 * NORON , sleep 10ms
 */
void ST7735::set_normal_mode()
{
    send_cmd(ST7735_NORON);
    sleep_ms(10);
}

// /**
//  * @brief init for "green tab" version of display ... I don't know why
//  */
// void ST7735::init_green_tab()
// {
//     // common init
//     init_wakeup();
//     init_frame_rate_control();
//     init_inversion_control();
//     init_power_control();
//     init_rotation_and_color();
//     // greentab specific
//     init_column_row_address();
//     // optional but improve color
//     init_gamma();
//     // start device
//     init_normal_display_on();
// }

void ST7735::send_cmd(uint8_t cmd)
{
    enable_command_pin(true);
    spi->single_write_8(cmd);
    enable_command_pin(false);
}

void ST7735::send_cmd_list(uint8_t *cmd, size_t len)
{
    enable_command_pin(true);
    spi->single_write_8(cmd[0]);
    enable_command_pin(false);
    for (size_t i = 1; i < len; i++)
        spi->single_write_8(cmd[i]);
}

void ST7735::send_buffer(uint8_t *buffer, size_t buffer_len)
{
    spi->burst_write_8(buffer, buffer_len);
    send_cmd(ST7735_NOP);
}

ST7735::ST7735(HW_SPI_Master *spi, struct_ConfigST7735 device_config)
    : GraphicDisplayDevice(0, 0)
{
    this->spi = spi;
    this->dc_pin = device_config.dc_pin;
    this->backlight_pin = device_config.backlight_pin;
    this->hw_reset_pin = device_config.hw_reset_pin;
    // init hardware pins
    init_pins();
    set_backlight(true);
    // common init
    device_wakeup();
    config_frame_rate_control();
    config_inversion_control();
    config_power_control();
    // device specific init
    config_device_specific_size_and_offsets(device_config);
    config_rotation_and_color(device_config);
    // init_column_row_address(device_config);
    // optional but improve color
    config_gamma();
    // start device
    set_normal_mode();
    set_display_ON();
}

void ST7735::config_device_specific_size_and_offsets(struct_ConfigST7735 device_config)
{
    switch (device_config.display_type)
    {
    case ST7735DisplayType::ST7735_144_128_RGB_128_GREENTAB:
        this->TFT_panel_width_in_pixel = 128;
        this->TFT_panel_height_in_pixel = 128;
        this->rgb_order = false;
        this->ST7735_device_column_offset = ST7735_144_128x128_column_offset;
        this->ST7735_device_row_offset = ST7735_144_128x128_row_offset;
        break;
    case ST7735DisplayType::ST7735_177_160_RGB_128_GREENTAB:
        this->rgb_order = true;
        this->ST7735_device_column_offset = ST7735_177_160x128_column_offset;
        this->ST7735_device_row_offset = ST7735_177_160x128_row_offset;
        switch (device_config.rotation)
        {
        case ST7735Rotation::_0:
            this->TFT_panel_width_in_pixel = 128;
            this->TFT_panel_height_in_pixel = 160;
            break;
        case ST7735Rotation::_90:
            this->TFT_panel_width_in_pixel = 160;
            this->TFT_panel_height_in_pixel = 128;
            break;
        case ST7735Rotation::_180:
            this->TFT_panel_width_in_pixel = 128;
            this->TFT_panel_height_in_pixel = 160;
            break;
        case ST7735Rotation::_270:
            this->TFT_panel_width_in_pixel = 160;
            this->TFT_panel_height_in_pixel = 128;
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
}

ST7735::~ST7735()
{
}

void ST7735::set_display_ON()
{
    send_cmd(ST7735_DISPON);
    sleep_ms(120);
}

void ST7735::set_display_OFF()
{
    send_cmd(ST7735_DISPOFF);
    sleep_ms(120);
}

void ST7735::enable_sleep(bool enable)
{
    send_cmd(enable ? ST7735_SLPIN : ST7735_SLPOUT);
    sleep_ms(120);
}

void ST7735::set_backlight(bool on)
{
    gpio_put(backlight_pin, on);
}

void ST7735::soft_reset()
{
    send_cmd(ST7735_SWRESET);
    sleep_ms(120);
}

void ST7735::set_RAM_write_addresses(uint8_t start_x, uint8_t start_y, size_t width, size_t height)
{
    uint16_t device_start_x = start_x + TFT_panel_start_x;
    uint16_t device_end_x = device_start_x + width - 1;
    uint16_t device_start_y = start_y + TFT_panel_start_y;
    uint16_t device_end_y = device_start_y + height - 1;

    send_cmd(ST7735_CASET);
    spi->single_write_16(device_start_x);
    spi->single_write_16(device_end_x);
    send_cmd(ST7735_RASET);
    spi->single_write_16(device_start_y);
    spi->single_write_16(device_end_y);
}

void ST7735::check_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg)
{
    // check limit of screen
    assert(framebuffer_cfg.widget_anchor_y + framebuffer_cfg.pixel_frame_height <= TFT_panel_height_in_pixel);
    assert(framebuffer_cfg.widget_anchor_x + framebuffer_cfg.pixel_frame_width <= TFT_panel_width_in_pixel);
}

void ST7735::clear_device_screen_buffer()
{
    uint8_t xsa = 0;
    uint8_t ysa = 0;
    size_t w = TFT_panel_width_in_pixel;
    size_t h = TFT_panel_height_in_pixel;
    set_RAM_write_addresses(xsa, ysa, w, h);
    send_cmd(ST7735_RAMWR);
    for (size_t i = 0; i < w * h; i++)
    {
        // ColorIndex color = ColorIndex::BLACK;
        // spi->single_write_16(color_palette[static_cast<int>(color)]);

        spi->single_write_16(0x0000);
    }
}

void ST7735::show(struct_PixelFrame *pixel_frame,
                  const uint8_t anchor_x, const uint8_t anchor_y)
{
}

void ST7735::clear_pixel_buffer(struct_PixelFrame *pixel_frame)
{
    memset(pixel_frame->pixel_frame_buffer, 0x00, pixel_frame->pixel_frame_buffer_size);
}

void ST7735::create_pixel_buffer(struct_PixelFrame *pixel_frame)
{
    pixel_frame->pixel_frame_buffer_size = pixel_frame->pixel_frame_width * pixel_frame->pixel_frame_height;

    pixel_frame->pixel_frame_buffer = new uint8_t[pixel_frame->pixel_frame_buffer_size];
    clear_pixel_buffer(pixel_frame);
}

void ST7735::pixel(struct_PixelFrame *pixel_frame,
                   const int x, const int y, const ColorIndex color)
{
}

void ST7735::draw_char_into_pixel(struct_PixelFrame *pixel_frame,
                                  const struct_ConfigTextWidget text_config,
                                  const char character,
                                  const uint8_t anchor_x, const uint8_t anchor_y)
{
}
/**
 * @brief init for "red tab" version of display ... I don't know why
 * _ wake up_
 * harware_reset
 * SW_reset, sleep 150us
 * SLPOUT, sleep 150us
 * _Frame rate control_
 * FRMCTR1 , [0x01, 0x2C, 0x2D] fastest refresh, 6 lines front, 3 lines back
 * FRMCTR2 , [0x01, 0x2C, 0x2D]
 * FRMCTR3 , [0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d], sleep 10us
 * _display inversion control_
 * INVCTR , [0x07]
 * _power control_
 * PWCTR1 , [0xA2,0x02,0x84]
 * PWCTR2 , [0xC5] #VGH = 14.7V, VGL = -7.35V
 * PWCTR3 , [0x0A, 0x00] opamp current small, boost frequency
 * PWCTR4 , [0x8A, 0x2A] opamp current small, boost frequency
 * PWCTR5 , [0x8A, 0xEE] opamp current small, boost frequency
 * VMCTR1 , [0x0E]
 * _set rotation and color_
 * ---> MADCTL , [rotation and RBG] [0xC8]
 *
 * COLMOD , [0x05]
 * _column row address set_
 * ---> CASET , [0x00,0x0_0_,0x00, width-1]
 * ---> RASET , [0x00,0x0_0_,0x00, height-1]
 *
 * _gamma_
 * ---> GMCTRP1 , [0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22, 0x1f, 0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10]
 * ---> GMCTRN1 , [0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e, 0x30, 0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10]
 * _set display normal ON
 * ---> DISPON
 * NORON , sleep 10us
 */
