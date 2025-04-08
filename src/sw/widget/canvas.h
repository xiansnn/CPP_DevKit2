/**
 * @file canvas.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-07
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "pico/stdlib.h"
#include <map>

/// @brief define the code value for color
enum class ColorIndex
{
    BLACK = 0, // "BLACK" must be coded with code 0x0 for monochrome display device
    WHITE = 1, // "WHITE" must be coded with code 0x1 for monochrome display device

    BLUE,    //     {ColorIndex::BLUE, {0x00, 0x00, 0xFF}},
    LIME,    //     {ColorIndex::LIME, {0x00, 0xFF, 0x00}},
    CYAN,    //     {ColorIndex::CYAN, {0x00, 0xFF, 0xFF}},
    RED,     //     {ColorIndex::RED, {0xFF, 0x00, 0x00}},
    YELLOW,  //     {ColorIndex::YELLOW, {0xFF, 0xFF, 0x00}},
    MAGENTA, //     {ColorIndex::MAGENTA, {0xFF, 0x00, 0xFF}},

    NAVY,     //     {ColorIndex::NAVY, {0x00, 0x00, 0x80}},
    GREEN,    //     {ColorIndex::GREEN, {0x00, 0x80, 0x00}},
    TEAL,     //     {ColorIndex::TEAL, {0x00, 0x80, 0x80}},
    BURGUNDY, //     {ColorIndex::BURGUNDY, {0x80, 0x00, 0x00}},
    PURPLE,   //     {ColorIndex::PURPLE, {0x80, 0x00, 0x80}},
    OLIVE,    //     {ColorIndex::OLIVE, {0x80, 0x80, 0x00}},
    GRAY,     //     {ColorIndex::GRAY, {0x80, 0x80, 0x80}},

    SILVER, //     {ColorIndex::SILVER, {0xC0, 0xC0, 0xC0}},
    MAROON, //     {ColorIndex::MAROON, {0xA5, 0x2A, 0x2A}},
    ORANGE, //     {ColorIndex::ORANGE, {0xFF, 0xA5, 0x00}},
    GOLD,   //     {ColorIndex::GOLD, {0xFF, 0xD7, 0x00}},
    FOREST  //     {ColorIndex::FOREST, {0x22, 0x8B, 0x22}}};
};

extern std::map<ColorIndex, uint16_t> color565_palette;

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

/// @brief data structure used to configure graphic framebuffer
struct struct_ConfigGraphicWidget
{

    /// @brief the frame width of the graphic frame
    size_t pixel_frame_width;
    /// @brief the frame height of the graphic frame
    size_t pixel_frame_height;
    /// @brief the foreground color
    ColorIndex fg_color{ColorIndex::WHITE};
    /// @brief  the background color
    ColorIndex bg_color{ColorIndex::BLACK};
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
    ColorIndex fg_color{ColorIndex::WHITE};
    /**
     * @brief The background color, defaul to BLACK
     */
    ColorIndex bg_color{ColorIndex::BLACK};
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
