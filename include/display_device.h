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
 * @brief This is the abstract class to handle all generic behavior of physical display devices (e.g. OLED screen SSD1306).
 * It derived from Framebuffer. This allows to draw text and graphics directly into the display framebuffer
 * thanks to Framebuffet class text and graphic primitives indepently from any kind of widget
 *
 */
class DisplayDevice
{
protected:
public:
    // FramebufferFormat frame_format;

    struct_PixelMemory pixel_memory;
    /**
     * @brief Construct a new Display Device object
     *
     * @param width The width of physical screen, in pixel.
     * @param height The height of physical screen, in pixel.
     * @param graph_cfg the graphical configuration data structure
     * @param format The framebuffer format ... see Framebuffer class FramebufferFormat enumeration
     */
    DisplayDevice(size_t width,
                  size_t height);

    /**
     * @brief Destroy the UIDisplayDevice object
     *
     */
    virtual ~DisplayDevice();

    /**
     * @brief Fill the pixel_buffer with "0" (BLACK). Reset also character position to (0,0).
     */
    virtual void clear_pixel_buffer(struct_PixelMemory *pixel_memory) = 0;

    /// @brief a common function where the pixel_buffer is created and deleted if already existing
    virtual void create_pixel_buffer(struct_PixelMemory *pixel_memory) = 0;

    /**
     * @brief This is an pure virtual member function that all final derived class must implement.
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

    /// @brief a graphic primitive to draw a character at a pixel position
    /// \note NOTICE: drawChar() implementation depends strongly on the FramebufferFormat.
    /// There should be one code for each format.
    /// @param font the font used to draw the character
    /// @param c the foreground color of the character
    /// @param anchor_x the pixel position on x-axis to start drawing the character (upper left corner)
    /// @param anchor_y the pixel position on y-axis to start drawing the character (upper left corner)
    virtual void drawChar(struct_PixelMemory *pixel_memory_structure, struct_TextFramebuffer *text_config, char c, uint8_t anchor_x, uint8_t anchor_y) = 0;
};

class TextDisplayDevice
{
private:
    size_t number_of_char_width;
    size_t number_of_char_height;
    size_t text_buffer_size;
    char *text_buffer = nullptr;

public:
    TextDisplayDevice(size_t number_of_char_width,
                      size_t number_of_char_hight);
    ~TextDisplayDevice();
    virtual void print(char *text_string) = 0;
};

