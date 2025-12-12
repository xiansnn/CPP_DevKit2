/**
 * @file display_device.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "pico/stdlib.h"
#include "sw/widget/canvas.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/// @brief Enumeration of display commands for display task management
enum class DisplayCommand
{
    /// @brief Command to clear the screen
    CLEAR_SCREEN,
    /// @brief Command to show an image
    SHOW_IMAGE
};

/// @brief A generic class for all display device
/// \ingroup view
class DisplayDevice
{
private:
    /* data */
public:
    DisplayDevice(/* args */);
    virtual ~DisplayDevice();
};

class rtos_DisplayDevice;
class rtos_Widget;

/// @brief  data structure used to queue widget data to send to the display task
struct struct_WidgetDataToGateKeeper
{
    /// @brief the command to be executed by the display task
    DisplayCommand command{DisplayCommand::SHOW_IMAGE};
    /// @brief the display device
    rtos_DisplayDevice *display = nullptr;
    /// @brief the widget to be displayed
    rtos_Widget *widget = nullptr;
};

/// @brief This is the abstract class to handle all generic behavior of physical graphic display devices (e.g. OLED screen SSD1306).
/// \ingroup view
class GraphicDisplayDevice : public DisplayDevice
{
protected:
public:
    /// @brief the physical width of the screen (in pixel)
    size_t TFT_panel_width_in_pixel;
    /// @brief the physical height of the screen (in pixel)
    size_t TFT_panel_height_in_pixel;

    /// @brief A pure virtual member function. Each device must implement this method and check the compatibility of the widget parameter with the its physical limitations.
    /// @param framebuffer_cfg the widget configuration data
    /// @param canvas_format the format of the canvas
    virtual void check_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg, CanvasFormat canvas_format) = 0;

    /**
     * @brief A pure virtual member function.
     * It transfers the pixel buffer to the a part of display screen buffer starting at the (anchor_x, anchor_y) coordinates of the screen , expressed in pixel.
     * This method takes into account the specific addressing scheme and memory structure of the actual display device.
     *
     * @param canvas a pointer to the canvas that contains the buffer to be displayed
     * @param anchor_x the x(horizontal) starting position of the frame within the display screen,(in pixel)
     * @param anchor_y the y(vertical) starting position of the frame within the display screen,(in pixel)
     */
    virtual void show(Canvas *canvas, const uint8_t anchor_x, const uint8_t anchor_y) = 0;

    /**
     * @brief Construct a new Display Device object
     *
     * @param screen_width The width of physical screen, in pixel
     * @param screen_height The height of physical screen, in pixel.
     */
    GraphicDisplayDevice(size_t screen_width,
                         size_t screen_height);

    /// @brief Destroy the Display Device object
    virtual ~GraphicDisplayDevice();
};

/**
 * @brief A class dedicated to pure text display such as console, printer, ASCII character line display
 * \ingroup view
 */
class TerminalConsole : public DisplayDevice
{
private:
public:
    /// @brief the size, in number of character of a line
    size_t number_of_column;
    /// @brief the number of line
    size_t number_of_line;
    /// @brief  the number of characters
    size_t text_buffer_size;
    /// @brief the effective character buffer
    char *text_buffer = nullptr;
    /**
     * @brief Construct a new Printer Device object
     *
     * @param number_of_char_width
     * @param number_of_char_height
     */
    TerminalConsole(size_t number_of_char_width,
                    size_t number_of_char_hieght);
    virtual ~TerminalConsole();

    /// @brief the method that actually print the content of text_buffer on the console
    virtual void show();
};

/// @brief The RTOS display device is the base class for all display devices that are managed by a dedicated display task in an RTOS environment.
/// \ingroup view
class rtos_DisplayDevice
{
private:
    /* data */
public:
    /// @brief the mutex to protect the display device access
    SemaphoreHandle_t display_device_mutex;
    rtos_DisplayDevice(/* args */);
    ~rtos_DisplayDevice();
    virtual void show_widget(rtos_Widget *widget_to_show) = 0;

    virtual void clear_device_screen_buffer() = 0;
};

/// @brief The RTOS graphic display device is the base class for all graphic display devices that are managed by a dedicated display task in an RTOS environment.
/// \ingroup view
class rtos_GraphicDisplayDevice : public rtos_DisplayDevice
{
private:
    /* data */
public:
    rtos_GraphicDisplayDevice(/* args */);
    ~rtos_GraphicDisplayDevice();

    virtual void check_rtos_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg, CanvasFormat canvas_format) = 0;
};
/// @brief The RTOS terminal console is the class for all text display devices that are managed by a dedicated display task in an RTOS environment.
/// \ingroup view
class rtos_TerminalConsole : public rtos_DisplayDevice
{
private:
    /* data */
public:
    /// @brief the size, in number of character of a line
    size_t number_of_column;
    /// @brief the number of line
    size_t number_of_line;
    /// @brief  the number of characters
    size_t text_buffer_size;


    void show_widget(rtos_Widget *widget_to_show);

    void clear_device_screen_buffer();
    rtos_TerminalConsole(size_t number_of_char_width,
                         size_t number_of_char_hight);
    virtual ~rtos_TerminalConsole();
};

class rtos_GraphicDisplayGateKeeper
{
private:
    SemaphoreHandle_t data_sent; // pour attendre la fin d'utilisation de la resource bus / display
public:
    QueueHandle_t graphic_widget_data;
    rtos_GraphicDisplayGateKeeper(/* args */);
    ~rtos_GraphicDisplayGateKeeper();
    void send_clear_device_command(rtos_GraphicDisplayDevice *device);
    void send_widget_data(rtos_Widget *widget);
    void receive_widget_data(struct_WidgetDataToGateKeeper received_widget_data);
};