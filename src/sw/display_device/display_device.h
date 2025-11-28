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

/// @brief data structure used to queue data to send to the display task
struct struct_DataToShow
{
    /// @brief the command to be executed by the display task
    DisplayCommand command{DisplayCommand::SHOW_IMAGE};
    /// @brief the display device
    DisplayDevice *display = nullptr;
    /// @brief the canvas to be displayed
    Canvas *canvas = nullptr;
    /// @brief the x anchor position of the canvas on the display
    uint8_t anchor_x = 0;
    /// @brief the y anchor position of the canvas on the display
    uint8_t anchor_y = 0;
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
     * @param number_of_char_hight
     */
    TerminalConsole(size_t number_of_char_width,
                    size_t number_of_char_hight);
    virtual ~TerminalConsole();

    /// @brief the method that actually print the content of text_buffer on the console
    virtual void show();


};


/// @brief The RTOS display device is the base class for all display devices that are managed by a dedicated display task in an RTOS environment.
class rtos_DisplayDevice
{
private:
    /* data */
public:
    /// @brief the queue handle to send data to the display task
    QueueHandle_t input_queue;
    /// @brief the mutex to protect the display device access   
    SemaphoreHandle_t display_device_mutex;
    rtos_DisplayDevice(/* args */);
    ~rtos_DisplayDevice();
};

/// @brief The RTOS graphic display device is the base class for all graphic display devices that are managed by a dedicated display task in an RTOS environment.
class rtos_GraphicDisplayDevice :public rtos_DisplayDevice
{
private:
    /* data */
public:
    rtos_GraphicDisplayDevice(/* args */);
    ~rtos_GraphicDisplayDevice();
};
/// @brief The RTOS terminal console is the class for all text display devices that are managed by a dedicated display task in an RTOS environment.
class rtos_TerminalConsole : public rtos_DisplayDevice
{
private:
    /* data */
public:
    /// @brief the method to send text buffer to a queue
    /// @param text_to_print
    void show_from_display_queue(char *text_to_print);
    rtos_TerminalConsole(/* args */);
    ~rtos_TerminalConsole();
};





// class rtos_DisplayGateKeeper
// {
// private:
//     QueueHandle_t text_buffer_queue;  // char * si printdevice ou struct_DataToShow si graphic dispaly
//     SemaphoreHandle_t data_sent; //pour attendre la fin d'utilisation de la resource bus / display
// public:
//     rtos_DisplayGateKeeper(/* args */);
//     ~rtos_DisplayGateKeeper();
// };

/*
---------------------------------PrintDevice
char *text_to_tprint;

    while (true)
    {
        xQueueReceive(text_buffer_queue, &text_to_tprint, portMAX_DELAY);
        p7.hi();
        my_serial_monitor.show_from_display_queue(text_to_tprint);
        p7.lo();
        xSemaphoreGive(data_sent);
    }
--------------------SSD1306-------------------------------------------
    struct_DataToShow received_data_to_show;

    while (true)
    {
        xQueueReceive(display_data_queue, &received_data_to_show, portMAX_DELAY);
        p4.hi();
        switch (received_data_to_show.command)
        {
        case DisplayCommand::SHOW_IMAGE:
            ((rtos_SSD1306 *)received_data_to_show.display)->show_from_display_queue(received_data_to_show);
            break;
        case DisplayCommand::CLEAR_SCREEN:
            ((rtos_SSD1306 *)received_data_to_show.display)->clear_device_screen_buffer();
            break;
        default:
            break;
        }

        xSemaphoreGive(data_sent);
        p4.lo();
-----------------------SSD1306---------------------------
    struct_SSD1306DataToShow received_data_to_show;

    while (true)
    {
        xQueueReceive(display_data_queue, &received_data_to_show, portMAX_DELAY);
        p4.hi();
        ((rtos_SSD1306 *)received_data_to_show.display)->show_render_area(received_data_to_show.data_buffer,
                                        received_data_to_show.display_area,
                                        received_data_to_show.addressing_mode);
        xSemaphoreGive(data_sent_to_I2C);
        p4.lo();

----------------------------ST7735---------------------------
    struct_DataToShow received_data_to_show;

    while (true)
    {
        xQueueReceive(display_queue_to_SPI, &received_data_to_show, portMAX_DELAY);
        p4.hi();
        switch (received_data_to_show.command)
        {
        case DisplayCommand::SHOW_IMAGE:
            ((rtos_ST7735 *)received_data_to_show.display)->show_from_display_queue(received_data_to_show);
            break;
        case DisplayCommand::CLEAR_SCREEN:
            ((rtos_ST7735 *)received_data_to_show.display)->clear_device_screen_buffer();
            break;
        default:
            break;
        }

        xSemaphoreGive(data_sent_to_SPI);

        p4.lo();




*/
