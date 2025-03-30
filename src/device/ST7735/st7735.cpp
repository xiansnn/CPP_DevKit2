#include "st7735.h"

void ST7735::command_pin_enable(bool enable)
{
    gpio_put(dc_pin, enable ? DCX_COMMAND : DCX_DATA);
}

void ST7735::hardware_reset()
{
    gpio_put(hw_reset_pin, 0);
    sleep_us(15);
    gpio_put(hw_reset_pin, 1);
    sleep_ms(120);
}

void ST7735::init_pins()
{
    gpio_init(dc_pin);
    gpio_set_dir(dc_pin, GPIO_OUT);
    gpio_pull_up(dc_pin);

    gpio_init(hw_reset_pin);
    gpio_set_dir(hw_reset_pin, GPIO_OUT);
    gpio_pull_up(hw_reset_pin);

    gpio_init(backlight_pin);
    gpio_set_dir(backlight_pin, GPIO_OUT);
}

/**
 * @brief _ wake up_
 * harware_reset
 * SW_reset, sleep 150us
 * SLPOUT, sleep 150us
 */
void ST7735::init_wakeup()
{
    hardware_reset();
    soft_reset();
    send_cmd(ST7735_SLPOUT);
}

/**
 * @brief _Frame rate control_
 * FRMCTR1 , [0x01, 0x2C, 0x2D] fastest refresh, 6 lines front, 3 lines back
 * FRMCTR2 , [0x01, 0x2C, 0x2D]
 * FRMCTR3 , [0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d], sleep 10us
 */
void ST7735::init_frame_rate_control()
{
}

/**
 * @brief  _display inversion control_
 * INVCTR , [0x07]
 */
void ST7735::init_inversion_control()
{
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
void ST7735::init_power_control()
{
}

/**
 * @brief  _set rotation and color_
 * MADCTL , [rotation and RBG]
 * COLMOD , [0x05]
 */
void ST7735::init_rotation_and_color()
{
}

/**
 * @brief  _column row address set_
 * CASET , [0x00,0x01,0x00, width-1]
 * RASET , [0x00,0x01,0x00, height-1]
 * we start column 1
 */
void ST7735::init_column_row_address()
{
}

/**
 * @brief _gamma_
 * GMCTRP1 , [0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10]
 * GMCTRN1 , [0x03, 0x1d, 0x07, 0x06, 0x2e, 0x2c, 0x29, 0x2d, 0x2e, 0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10]
 */
void ST7735::init_gamma()
{
}

/**
 * @brief  _set display normal ON
 * NORON , sleep 10us
 */
void ST7735::init_normal_display_on()
{
}

/**
 * @brief init for "green tab" version of display ... I don't know why
 */
void ST7735::init_green_tab()
{
    init_wakeup();
    init_frame_rate_control();
    init_inversion_control();
    init_power_control();
    init_rotation_and_color();
    init_column_row_address();
    init_gamma();
    init_normal_display_on();
}


void ST7735::send_cmd(uint8_t cmd)
{
    command_pin_enable(true);
    spi->single_byte_write(cmd);
    command_pin_enable(false);
}

void ST7735::send_cmd_list(uint8_t cmd, size_t len)
{
}

void ST7735::send_buffer(uint8_t *buffer, size_t buffer_len)
{
}

ST7735::ST7735(HW_SPI_Master *spi, struct_ConfigST7735 device_config)
    : GraphicDisplayDevice(device_config.device_width, device_config.device_height)
{
    this->spi = spi;
    this->dc_pin = device_config.dc_pin;
    this->backlight_pin = device_config.backlight_pin;
    this->hw_reset_pin = device_config.hw_reset_pin;
    init_pins();
    set_backlight(true);
    hardware_reset();
}

ST7735::~ST7735()
{
}

void ST7735::display_on(bool on)
{
    uint8_t cmd = on ? ST7735_DISPON : ST7735_DISPOFF;
    send_cmd(cmd);
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

void ST7735::check_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg)
{
    // check limit of screen
    assert(framebuffer_cfg.widget_anchor_y + framebuffer_cfg.pixel_frame_height <= this->screen_pixel_height);
    assert(framebuffer_cfg.widget_anchor_x + framebuffer_cfg.pixel_frame_width <= this->screen_pixel_width);
}

void ST7735::show(struct_PixelFrame *pixel_frame,
                  const uint8_t anchor_x, const uint8_t anchor_y)
{
}

void ST7735::clear_pixel_buffer(struct_PixelFrame *pixel_frame)
{
}

void ST7735::create_pixel_buffer(struct_PixelFrame *pixel_frame)
{
}

void ST7735::pixel(struct_PixelFrame *pixel_frame,
                   const int x, const int y, const PixelColor color)
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
