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

/// @brief define the binary value for color (limited here to monochome, can be extended later)
enum class PixelColor
{
    /// @brief black coded with binary value 0x0
    BLACK = 0,
    /// @brief white coded with binary value 0x1
    WHITE = 1
};

/// @brief the data structure that describes framebuffer
struct struct_PixelMemory
{
    /// @brief The number of pixel along the width of the frame.
    uint8_t frame_width;

    /// @brief The number of pixel along the height of the frame.
    uint8_t frame_height;

    /// @brief the buffer where graphic are drawn
    uint8_t *pixel_buffer = nullptr;

    /// @brief size of the buffer that contains graphics as map of pixels.
    size_t pixel_buffer_size;
};

/// @brief the data sttructure used to configure textual widget
struct struct_TextFramebuffer
{
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
};

/**
 * @brief the generic abstract class for display devices
 * 
 */
class DisplayDevice
{
private:
    /* data */
public:
    DisplayDevice();
    ~DisplayDevice();
};

/**
 * @brief This is the abstract class to handle all generic behavior of physical graphic display devices (e.g. OLED screen SSD1306).
 * It derived from GraphicFramebuffer. This allows to draw graphics directly into the display framebuffer
 * thanks to Framebuffet class graphic primitives indepently from any kind of widget
 *
 */
class GraphicDisplayDevice : public DisplayDevice
{
protected:
public:
    /// @brief the data structure of the pixel buffer
    struct_PixelMemory pixel_memory;

    /**
     * @brief Construct a new Display Device object
     *
     * @param width The width of physical screen, in pixel
     * @param height The height of physical screen, in pixel.
     */
    GraphicDisplayDevice(size_t width,
                         size_t height);

    /**
     * @brief Destroy the Display Device object
     *
     */
    virtual ~GraphicDisplayDevice();

    /**
     * @brief A pure virtual method. Fill the pixel_buffer with "0" (BLACK). Reset also character position to (0,0).
     * Usefull when we have a graphic framework
     *
     * @param pixel_memory the pixel buffer to fill
     */
    virtual void clear_pixel_buffer(struct_PixelMemory *pixel_memory) = 0;

    /**
     * @brief A pure virtual method. Create a pixel buffer object.
     * \note : the width and height of the pixel frame must be known before invoking this method.
     * \note : the graphic framebuffer uses this method to create its pixel frame buffer.
     *
     * @param pixel_memory the pixel buffer to complete
     */
    virtual void create_pixel_buffer(struct_PixelMemory *pixel_memory) = 0;

    /**
     * @brief A pure virtual member function.
     * It transfers the framebuffer buffer to the a part of display screen buffer starting at the (anchor_x, anchor_y) coordinates of the screen , expressed in pixel.
     *
     * @param pixel_memory_structure a pointer to the struct_PixelMemory that contains the pixel_buffer to be displayed
     * @param anchor_x the x (horizontal)starting position of the frame within the display screen, (in pixel)
     * @param anchor_y the y (vertical) starting position of the frame within the display screen, (in pixel)
     */
    virtual void show(struct_PixelMemory *pixel_memory_structure, uint8_t anchor_x, uint8_t anchor_y) = 0;

    /**
     * @brief the graphic primitive to draw a pixel
     *
     * @param pixel_memory_structure
     * @param x the x position of the pixel
     * @param y the y position of the pixel
     * @param c the color of the pixel
     */
    virtual void pixel(struct_PixelMemory *pixel_memory_structure, int x, int y, PixelColor c = PixelColor::WHITE) = 0;

    /**
     * @brief a graphic primitive to draw a character at a pixel position
     * \note : DrawChar() implementation depends strongly on the FramebufferFormat.
     *
     * @param pixel_memory_structure
     * @param text_config the configuration file of the text framebuffer
     * @param c the character to draw
     * @param anchor_x the pixel position on x-axis to start drawing the character (upper left corner)
     * @param anchor_y the pixel position on y-axis to start drawing the character (upper left corner)
     */
    virtual void drawChar(struct_PixelMemory *pixel_memory_structure, struct_TextFramebuffer *text_config, char c, uint8_t anchor_x, uint8_t anchor_y) = 0;
};

/// @brief A pure textual class.
class TextDisplayDevice : public GraphicDisplayDevice
{
private:
    /// @brief the size, in number of character of a line
    size_t number_of_char_width;
    /// @brief the number of line
    size_t number_of_char_height;
    /// @brief  the number of characters
    size_t text_buffer_size;
    /// @brief the effective character buffer
    char *text_buffer = nullptr;

public:
    /**
     * @brief Construct a new Text Display Device object
     *
     * @param number_of_char_width the size, in number of character of a line
     * @param number_of_char_hight the number of line
     */
    TextDisplayDevice(size_t number_of_char_width,
                      size_t number_of_char_hight);
    ~TextDisplayDevice();
    /**
     * @brief A pure virtual method. Simply copy the text characters to the text buffer
     *
     * @param text_string the character string
     */
    virtual void print(char *text_string) = 0;
};
