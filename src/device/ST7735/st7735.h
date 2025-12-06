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
#include "hw/spi/rtos_hw_spi.h"
#include "sw/display_device/display_device.h"
#include "sw/widget/rtos_widget.h"

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
#define ST7735_177_160x128_column_offset 0 // 2
/// @brief hardware row offset between the TFT panel and the IC ST7735
#define ST7735_177_160x128_row_offset 0 // 1

/// @brief tag used to ru the correct init code
enum class ST7735DisplayType
{
    /// @brief LCD TFT 1"44 128(RGB)x128 green_tab configuration
    ST7735_144_128_RGB_128_GREENTAB, //
                                     /// @brief LCD TFT 1"77 160(RGB)x128 green_tab configuration
    ST7735_177_160_RGB_128_GREENTAB //
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
protected:
    /// @brief SPI driver associated with the display
    HW_SPI_Master *spi;
    /// @brief the GPIO connected to the Data/Command_ input
    uint dc_pin;
    /// @brief the GPIO connected to the backlight input
    uint backlight_pin;
    /// @brief the GPIO connected to the hardware reset input
    uint hw_reset_pin;
    /// @brief the column offset for the ST7735 device
    uint8_t ST7735_device_column_offset{0};
    /// @brief the row offset for the ST7735 device
    uint8_t ST7735_device_row_offset{0};
    /// @brief the x starting position of the TFT panel within the ST7735 memory
    uint8_t TFT_panel_start_x{0};
    /// @brief the y starting position of the TFT panel within the ST7735 memory
    uint8_t TFT_panel_start_y{0};
    /// @brief a flag that indicates if the color order is RGB (true) or BGR (false)
    bool rgb_order;

    /// @brief enable or disable the command mode (DC pin low or high)
    /// @param enable true for command mode, false for data mode
    void enable_command_pin(bool enable);
    /// @brief hardware reset of the ST7735 device
    void hardware_reset();
    /// @brief initialize pins for the ST7735 device
    void init_pins();
    /// @brief wake up the device from sleep mode
    void device_wakeup();
    /// @brief config framerate control
    void config_frame_rate_control();
    /// @brief config display inversion control
    void config_inversion_control();
    /// @brief config power control
    void config_power_control();
    // void init_column_row_address(struct_ConfigST7735 device_config);
    void config_gamma();
    void set_normal_mode();
    // void init_green_tab();
    /// @brief config specific device size and offsets
    /// @param device_config the device configuration file
    void config_device_specific_size_and_offsets(struct_ConfigST7735 device_config);
    /// @brief send a command to the ST7735 device
    /// @param cmd the command to be sent
    void send_cmd(uint8_t cmd);
    /// @brief send a list of command to the ST7735 device
    /// @param cmd a pointer to the list of command
    /// @param len the length of the command list
    void send_cmd_list(uint8_t *cmd, size_t len);
    /// @brief send a buffer of data to the ST7735 device
    /// @param buffer a pointer to the buffer
    /// @param buffer_len the length of the buffer
    void send_buffer(uint8_t *buffer, size_t buffer_len);
    /// @brief set the backlight of the display
    /// @param on
    void set_backlight(bool on);
    /// @brief perform a software reset of the display
    void soft_reset();
    /// @brief set the RAM write addresses for the display
    /// @param start_x the starting x coordinate
    /// @param start_y the starting y coordinate
    /// @param width the width of the area to write
    /// @param height the height of the area to write
    void set_RAM_write_addresses(uint8_t start_x, uint8_t start_y, size_t width, size_t height);

public:
    /// @brief Construct a new ST7735 object
    /// @param spi the SPI associated driver
    /// @param device_config the device configuration file
    ST7735(HW_SPI_Master *spi, struct_ConfigST7735 device_config);
    virtual ~ST7735();

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
    void check_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg, CanvasFormat canvas_format);

    /// @brief fill the internal ST7735 screen buffer with the given color.
    /// Default to BLACK, clear the internal buffer
    /// @param color_index
    virtual void clear_device_screen_buffer(ColorIndex color_index = ColorIndex::BLACK);

    
    virtual void show(Canvas *canvas, const uint8_t anchor_x, const uint8_t anchor_y);
};

/// @brief RTOS version of the ST7735 display device
/// \ingroup view
class rtos_ST7735 : public ST7735 , public rtos_GraphicDisplayDevice
{
    private:
    /* data */
    public:
    /// @brief Construct a new rtos_ST7735 object
    /// @param spi the rtos_HW_SPI_Master associated driver
    /// @param device_config    the device configuration file
    rtos_ST7735(rtos_HW_SPI_Master *spi, struct_ConfigST7735 device_config);
    ~rtos_ST7735();

    /// @brief Show data from the display queue.
    /// @param data_to_show The data to display. 
    void show_from_display_queue(struct_WidgetDataToGateKeeper data_to_show);
    void show_widget(rtos_GraphicWidget* widget);
    void show(Canvas *canvas, const uint8_t anchor_x, const uint8_t anchor_y);

    /// @brief Clear the device screen buffer.
    void clear_device_screen_buffer();

    void check_rtos_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg, CanvasFormat canvas_format);
    
    /// @brief Send a clear screen command to the display gate keeper task.
    /// @param display_queue_to_SPI The queue used to send data to the display gate keeper task.
    /// @param sending_done A semaphore to signal when the sending is done.
    void send_clear_device_command(QueueHandle_t display_queue_to_SPI, SemaphoreHandle_t sending_done);
};
