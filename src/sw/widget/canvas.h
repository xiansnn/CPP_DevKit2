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
#include <stdio.h>
#include <map>

/// @brief define the code value for color
enum class ColorIndex
{
    /// @cond
    BLACK = 0, // "BLACK" must be coded with code 0x0 for monochrome display device
    WHITE = 1, // "WHITE" must be coded with code 0x1 for monochrome display device

    BLUE, //     {ColorIndex::BLUE, {0x00, 0x00, 0xFF}},
    LIME, //     {ColorIndex::LIME, {0x00, 0xFF, 0x00}},
    RED,  //     {ColorIndex::RED, {0xFF, 0x00, 0x00}},

    CYAN,    //     {ColorIndex::CYAN, {0x00, 0xFF, 0xFF}},
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

/// @brief the map color for RGB565 canvas
extern std::map<ColorIndex, uint16_t> color565_palette;

/// @brief the format of the canvas
enum class CanvasFormat
{
    /// @brief monochrome canvas, pixel arranged vertically, LSB is top pixel.
    /// @note example: SSD1306
    MONO_VLSB,
    /// @brief monochrome canvas, pixel arranged horizontally, LSB is left pixel
    /// @note example: SH1106
    MONO_HLSB,
    /// @brief monochrome canvas, pixel arranged horizontally, MSB is left pixel
    /// @note example: ST7735 in 1-bit mode
    MONO_HMSB,
    /// @brief color canvas, 16bits/pixel arranged 5b-red,6b-green,5b-blue
    /// @note example: ST7735 in 16-bit mode
    RGB565,
    /// @brief color canvas, 16bits/pixel arranged 5b-red,6b-green,5b-blue but with true RGB565 stored in buffer. This is to allow the use of 16bit DMA.
    /// @note example: ST7735 in 16-bit mode with transfer from canvas done by DMA. In this case the canvas buffer format Canvas::trueRGB565 is directly compatible with the ST7735 16-bit mode.
    trueRGB565
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
    /// @brief The font used. Current font are defined according to IBM CP437. The font files are derived from https://github.com/Harbys/pico-ssd1306 works.
    /// They come is size 5x8, 8x8, 12x16 and 16x32.
    const unsigned char *font{nullptr};
    /// @brief The number of space that ASCII character HT (aka TAB , "\t", 0x9) generates, default to 2
    uint8_t tab_size{2};
    /// @brief The foreground color, default to WHITE
    ColorIndex fg_color{ColorIndex::WHITE};
    /// @brief The background color, default to BLACK
    ColorIndex bg_color{ColorIndex::BLACK};
    /// @brief Wrap flag : if true, text wrap to the next line when end of line is reached.
    bool wrap{true};
    /// @brief auto_next_char flag : if true each char steps one position after being written.
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
    /// @brief the actual format of the canvas
    CanvasFormat canvas_format;
    /// @brief a copy of the widget foreground color
    ColorIndex canvas_fg_color;
    /// @brief a copy of the widget background color
    ColorIndex canvas_bg_color;

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
    size_t canvas_buffer_size_byte;
    
    /// @brief the size (in pixel) of the buffer
    size_t canvas_buffer_size_pixel;

    /// @brief the 8bit canvas buffer
    uint8_t *canvas_buffer{nullptr};

    /// @brief the 16bit canvasbuffer
    uint16_t *canvas_16buffer{nullptr};

    /// @brief Construct a new Canvas object
    /// @param canvas_width_pixel Width of the canvas (in pixel)
    /// @param canvas_height_pixel height of the canvas(in pixel)
    Canvas(uint8_t canvas_width_pixel,
           uint8_t canvas_height_pixel);
    virtual ~Canvas();

    /// @brief fill the canvas buffer with 0x00
    virtual void clear_canvas_buffer();

    /// @brief the graphic primitive to draw a pixel
    /// @param x the x position of the pixel
    /// @param y the y position of the pixel
    /// @param color the color of the pixel
    virtual void draw_pixel(const int x, const int y,
                            const ColorIndex color = ColorIndex::WHITE) = 0;
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
};

/// @brief A special version of canvas for color widget (and device) with 1 pixel/byte according to 565 RGB color coding
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
    ///\note the conversion from color index to RGB565 is done by the device after calling the show() member
    /// @param color
    void fill_canvas_with_color(ColorIndex color);

    void draw_pixel(const int x, const int y,
                    const ColorIndex color = ColorIndex::WHITE);
};
/// @brief A special version of canvas for color widget (and device) with true RGB565 color coding(i.e. 16bit) per pixel
///\ingroup view
class CanvasTrueRGB : public Canvas
{
private:
    void create_canvas_buffer();

public:
    /// @brief Construct a new Canvas R G B object
    /// @param canvas_width_pixel
    /// @param canvas_height_pixel
    CanvasTrueRGB(uint8_t canvas_width_pixel,
                  uint8_t canvas_height_pixel);
    ~CanvasTrueRGB();

    /// @brief fill the 16bit canvas buffer with 0x00
    virtual void clear_canvas_buffer();


    /// @brief fill the canvas buffer with the given color index
    ///\note the conversion from color index to RGB565 is done by the device after calling the show() member
    /// @param color
    void fill_canvas_with_color(ColorIndex color);

    void draw_pixel(const int x, const int y,
                    const ColorIndex color = ColorIndex::WHITE);
};

/// @brief A special version of canvas for monochrome widget with 8pixel/byte arranged horizontally.
/// Usefull for monochrome widget (e.g.text) even for color RGB565 display device
///\ingroup view
class CanvasHMSB : public Canvas
{
private:
    void create_canvas_buffer();

public:
    /// @brief constructor for CanvasHMSB
    /// @param canvas_width_pixel
    /// @param canvas_height_pixel
    CanvasHMSB(uint8_t canvas_width_pixel,
               uint8_t canvas_height_pixel);
    ~CanvasHMSB();
    /// @brief fill the canvas buffer with 0x00 (i.e. BLACK) of 0xFF (WHITE)
    ///\note the conversion from color bit (0b0 or 0b1) to RGB565 is done by the device after calling the show() member according to fg_color and bg_color
    /// @param color
    void fill_canvas_with_color(ColorIndex color);

    void draw_pixel(const int x, const int y,
                    const ColorIndex color = ColorIndex::WHITE);
};
