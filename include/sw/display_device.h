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

/// @brief index of the font width value in the <...>_font.h file
#define FONT_WIDTH_INDEX 0
/// @brief index of the font height value in the <...>_font.h file
#define FONT_HEIGHT_INDEX 1

/// @brief define the code value for color (limited here to monochome, can be extended later)
enum class PixelColor
{
    /// @brief "black" coded with code 0x0
    BLACK = 0,
    /// @brief "white" coded with code 0x1
    WHITE = 1
};

/// @brief the data structure that characterise the graphic framebuffer
struct struct_PixelFrame
{
    /// @brief The number of pixel along the width of the frame.
    uint8_t pixel_frame_width;
    /// @brief The number of pixel along the height of the frame.
    uint8_t pixel_frame_height;
    /// @brief size of the buffer that contains graphics as map of pixels.
    size_t pixel_frame_buffer_size;
    /// @brief the buffer where graphic are drawn
    uint8_t *pixel_frame_buffer = nullptr;
};

/// @brief the data structure that characterise the text buffer
struct struct_TextFrame
{
    /// @brief the size, in number of character of a line
    size_t number_of_column;
    /// @brief the number of line
    size_t number_of_line;
    /// @brief  the number of characters
    size_t text_buffer_size;
    /// @brief the effective character buffer
    char *text_buffer = nullptr;
};

/**
 * @brief data structure used to configure graphic framebuffer
 *
 */
struct struct_ConfigGraphicWidget
{

    /// @brief the frame width of the graphic frame
    size_t pixel_frame_width;
    /// @brief the frame height of the graphic frame
    size_t pixel_frame_height;
    /// @brief the foreground color
    PixelColor fg_color{PixelColor::WHITE};
    /// @brief  the background color
    PixelColor bg_color{PixelColor::BLACK};
    /// @brief the x_axis anchor of the widget
    uint8_t widget_anchor_x{0};
    /// @brief the y-axis anchor of the widget
    uint8_t widget_anchor_y{0};
    /// @brief a flag that indicates if the widget has a 1-pixel width border
    bool widget_with_border{false};
};

/// @brief the data structure used to configure textual widget
struct struct_ConfigTextWidget
{
    /// @brief The max number of line with respect to frame height and font height
    uint8_t number_of_column{0};
    /// @brief The max number of column with respect to frame width and font width
    uint8_t number_of_line{0};
    /// @brief the x_axis anchor of the widget
    uint8_t widget_anchor_x{0};
    /// @brief the y-axis anchor of the widget
    uint8_t widget_anchor_y{0};
    /**
     * @brief The font used. Current font are defined according to IBM CP437. The font files are derived from https://github.com/Harbys/pico-ssd1306 works.
     * They come is size 5x8, 8x8, 12x16 and 16x32.
     */
    const unsigned char *font{nullptr};
    /**
     * @brief  The number of space that ASCII character HT (aka TAB , "\t", 0x9) generates, default to 2
     */
    uint8_t tab_size{2};
    /**
     * @brief The foreground color, default to WHITE
     */
    PixelColor fg_color{PixelColor::WHITE};
    /**
     * @brief The background color, defaul to BLACK
     */
    PixelColor bg_color{PixelColor::BLACK};
    /**
     * @brief Wrap flag : if true, text wrap to the next line when end of line is reached.
     */
    bool wrap{true};
    /**
     * @brief auto_next_char flag : if true each char steps one position after being written.
     */
    bool auto_next_char{true};
    /// @brief a flag that indicates if the widget has a 1-pixel width border
    bool widget_with_border{false};
};

/// @brief A generic class for all display device
class DisplayDevice
{
private:
    /* data */
public:
    DisplayDevice(/* args */);
    ~DisplayDevice();
};


/// @brief This is the abstract class to handle all generic behavior of physical graphic display devices (e.g. OLED screen SSD1306).
class GraphicDisplayDevice : public DisplayDevice
{
protected:
public:
    /// @brief the physical width of the screen (in pixel)
    size_t screen_pixel_width;
    /// @brief the physical height of the screen (in pixel)
    size_t screen_pixel_height;

    /**
     * @brief A pure virtual member function. Each device must implement this method and check the compatibility of the widget parameter with the its physical limitations.
     *
     * @param framebuffer_cfg the widget configuration data
     */
    virtual void check_display_device_compatibility(struct_ConfigGraphicWidget framebuffer_cfg) = 0;

    /**
     * @brief A pure virtual member function.
     * It transfers the pixel buffer to the a part of display screen buffer starting at the (anchor_x, anchor_y) coordinates of the screen , expressed in pixel.
     * This method takes into account the specific addressing scheme and memory structure of the actual display device.
     *
     * @param pixel_frame a pointer to the struct_PixelFrame that contains the pixel_buffer to be displayed
     * @param anchor_x the x(horizontal) starting position of the frame within the display screen,(in pixel)
     * @param anchor_y
     */
    virtual void show(struct_PixelFrame *pixel_frame, const uint8_t anchor_x, const uint8_t anchor_y) = 0;

    /**
     * @brief Construct a new Display Device object
     *
     * @param screen_width The width of physical screen, in pixel
     * @param screen_height The height of physical screen, in pixel.
     */
    GraphicDisplayDevice(size_t screen_width,
                         size_t screen_height);

    /**
     * @brief Destroy the Display Device object
     *
     */
    virtual ~GraphicDisplayDevice();

    /**
     * @brief A pure virtual method. Usefull when to clear the pixel buffer.
     * It fills the pixel_buffer with "0" (BLACK). Reset also character position to (0,0).
     * @param pixel_frame the pixel buffer to fill
     */
    virtual void clear_pixel_buffer(struct_PixelFrame *pixel_frame) = 0;

    /**
     * @brief A pure virtual method. Create a pixel buffer object.
     * \note : the width and height of the pixel frame must be known before invoking this method.
     * \note : the graphic widget uses this method to create its pixel frame buffer.
     *
     * @param pixel_frame the pixel buffer to complete
     */
    virtual void create_pixel_buffer(struct_PixelFrame *pixel_frame) = 0;

    /**
     * @brief the graphic primitive to draw a pixel
     *
     * @param pixel_frame
     * @param x the x position of the pixel
     * @param y the y position of the pixel
     * @param color the color of the pixel
     */
    virtual void pixel(struct_PixelFrame *pixel_frame,
                       const int x, const int y,
                       const PixelColor color = PixelColor::WHITE) = 0;

    /**
     * @brief a graphic primitive to draw a character at a pixel position
     * \note : DrawChar() implementation depends strongly on the memory structure of the display device.
     *
     * @param pixel_frame
     * @param text_config the configuration file of the text framebuffer
     * @param character the character to draw
     * @param anchor_x the pixel position on x-axis to start drawing the character (upper left corner)
     * @param anchor_y the pixel position on y-axis to start drawing the character (upper left corner)
     */
    virtual void draw_char_into_pixel(struct_PixelFrame *pixel_frame,
                                      const struct_ConfigTextWidget text_config,
                                      const char character,
                                      const uint8_t anchor_x, const uint8_t anchor_y) = 0;
};

/**
 * @brief A class dedicated to pure text display such as console, printer, ASCII character line display
 *
 */
class PrinterDevice : public DisplayDevice
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
    // /// @brief the data structure of the text memory
    // struct_TextFrame text_memory;
    /**
     * @brief Construct a new Printer Device object
     *
     * @param number_of_char_width
     * @param number_of_char_hight
     */
    PrinterDevice(size_t number_of_char_width,
                  size_t number_of_char_hight);
    ~PrinterDevice();

    /// @brief the method that actually print the content of text_buffer on the console
    virtual void show();
};
