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

/// @brief index of the font width value in the <...>_font.h file
#define FONT_WIDTH_INDEX 0
/// @brief index of the font height value in the <...>_font.h file
#define FONT_HEIGHT_INDEX 1
///@brief the symbolic value of a byte
#define BYTE_SIZE 8

#include "pico/stdlib.h"
#include <map>

/// @brief define the code value for color
enum class ColorIndex
{
    /// @cond
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
            ///@endcond
};

/**
 * @brief the map color for RGB565 canvas
 *
 */
extern std::map<ColorIndex, uint16_t> color565_palette;

/**
 * @brief the format of the canvas
 *
 */
enum class CanvasFormat
{
    /// @brief monochrome canvas, pixel arranged vertically, LSB is top pixel
    MONO_VLSB,
    /// @brief monochrome canvas, pixel arranged horizontally, LSB is left pixel
    MONO_HLSB,
    /// @brief monochrome canvas, pixel arranged horizontally, MSB is left pixel
    MONO_HMSB,
    /// @brief color canvas, 16bits/pixel arranged 5b-red,6b-green,5b-blue
    RGB565
};

/// @brief the data structure that characterises the graphic canvas
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

/// @brief the data structure that characterises the text buffer
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

/// @brief The canvas is a virtual memory in which the widget draws.
///\ingroup view
class Canvas
{
protected:
public:

    /// @brief Create a canvas buffer object
    virtual void create_canvas_buffer() = 0;

    /// @brief fill the canvas with a given color
    /// @param color
    virtual void fill_canvas_with_color(ColorIndex color) = 0;

    /// @brief the width (in pixel) of the canvas and also of those of the associated widget
    uint8_t canvas_width_pixel;

    /// @brief the height (in pixel) of the canvas and also of those of the associated widget
    uint8_t canvas_height_pixel;

    /// @brief the size (in bytes) of the buffer
    size_t canvas_buffer_size;

    /// @brief the buffer
    uint8_t *canvas_buffer;

    /// @brief Construct a new Canvas object
    /// @param canvas_width_pixel Width of the canvas (in pixel)
    /// @param canvas_height_pixel height of the canvas(in pixel)
    Canvas(uint8_t canvas_width_pixel,
           uint8_t canvas_height_pixel);
    ~Canvas();

    /// @brief fill the canvas buffer with 0x00
    void clear_canvas_buffer();

    /// @brief return a pixelFrame structure
    ///\deprecated
    /// @return
    struct_PixelFrame get_pixel_frame(); //

    /// @brief the graphic primitive to draw a pixel
    /// @param x the x position of the pixel
    /// @param y the y position of the pixel
    /// @param color the color of the pixel
    virtual void draw_pixel(const int x, const int y,
                            const ColorIndex color = ColorIndex::WHITE) = 0;


    /// @brief a graphic primitive to draw a character at a pixel position
    /// @param text_config the configuration file of the text framebuffer
    /// @param character the character to draw
    /// @param anchor_x the pixel position on x-axis to start drawing the character (upper left corner)
    /// @param anchor_y the pixel position on y-axis to start drawing the character (upper left corner)
    virtual void draw_glyph(const struct_ConfigTextWidget text_config,
                            const char character,
                            const uint8_t anchor_x, const uint8_t anchor_y) = 0;
};

/// @brief A special version of canvas for monochrome widget (and device) with 8pixel/byte arranged vertically
///\ingroup view
class CanvasVLSB : public Canvas
{
private:
    void create_canvas_buffer();

public:
    /// @brief Construct a new Canvas V L S B object
    /// @param canvas_width_pixel
    /// @param canvas_height_pixel
    CanvasVLSB(uint8_t canvas_width_pixel,
               uint8_t canvas_height_pixel);
    ~CanvasVLSB();

    /// @brief fill the canvas buffer with 0x00 (i.e. BLACK) of 0xFF (WHITE)
    /// @param color
    void fill_canvas_with_color(ColorIndex color);

    void draw_pixel(const int x, const int y,
                    const ColorIndex color = ColorIndex::WHITE);

    void draw_glyph(const struct_ConfigTextWidget text_config,
                    const char character,
                    const uint8_t anchor_x, const uint8_t anchor_y);
};
/// @brief A special version of canvas for color widget (and device) with 1 pixel/byte accoeding to 565 RGB color coding
///\ingroup view
class CanvasRGB : public Canvas
{
private:
    void create_canvas_buffer();

public:
    /// @brief Construct a new Canvas R G B object
    /// @param canvas_width_pixel
    /// @param canvas_height_pixel
    CanvasRGB(uint8_t canvas_width_pixel,
              uint8_t canvas_height_pixel);
    ~CanvasRGB();

    /// @brief fill the canvas buffer with the given color index
    ///\note the conversion from color index to RGB565 is done by the device after calling the show() memeber
    /// @param color
    void fill_canvas_with_color(ColorIndex color);

    void draw_pixel(const int x, const int y,
                    const ColorIndex color = ColorIndex::WHITE);
    void draw_glyph(const struct_ConfigTextWidget text_config,
                    const char character,
                    const uint8_t anchor_x, const uint8_t anchor_y);
};
