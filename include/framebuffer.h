/**
 * @file framebuffer.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <string>
#include "pico/stdlib.h"
#include "display_device.h"
/// @brief flag used to generate 127 character font or full extended 255 character font.
/// \note Must be placed before include "...font..."
#define SSD1306_ASCII_FULL
#include "ssd1306/5x8_font.h"
#include "ssd1306/8x8_font.h"
#include "ssd1306/12x16_font.h"
#include "ssd1306/16x32_font.h"

/// @brief character code for BACKSPACE ('BS', 0x08)
#define BACKSPACE '\b'
/// @brief character code for HORIZONTAL_TAB ('HT', 0x09)
#define HORIZONTAL_TAB '\t'
/// @brief character code for LINE_FEED ('LF', 0x0A)
#define LINE_FEED '\n'
/// @brief character code for VERTICAL_TAB ('VT', 0x0B)
#define VERTICAL_TAB '\v'
/// @brief character code for FORM_FEED ('FF', 0x0C)
#define FORM_FEED '\f'
/// @brief character code for CARRIAGE_RETURN ('CR', 0x0D)
#define CARRIAGE_RETURN '\r'

/**
 * @brief Framebuffer is the basic framework to handle texts and graphics on a digital display.
 *
 * This framebuffer implementation is derived from the mycropython API and more explanations can be found
 * there https://docs.micropython.org/en/latest/library/framebuf.html#module-framebuf.
 *
 * The current implementation supports only MONO_VLSB frame_format (monochrome display with vertically addressed pixel LSB being on top)
 *  and tested with OLED SSD1306.
 *
 * The core of framebuffer is the pixel_buffer, a memory space that contains pixel values. This pixel_buffer is computed and initialized by the framebuffer constructor.
 *
 * Optionnally, when framebuffer contains text, a text_buffer that contains characters chain is used. It is created and initialized by
 *  update_text_buffer function member.
 * The configuration of this buffer is defined by struct_FramebufferText.
 *
 *
 */

class GraphicFramebuffer

{
private:
    /// @brief the graphic primitive to draw an ellipse \bug //FIXME doesn't work !
    /// @param x_center the x coordinate of the center
    /// @param y_center the y coordinate of the center
    /// @param x_radius the radius along x axis
    /// @param y_radius the radius along y axis
    /// @param fill a flag that indicates whether the ellipse is filled or not
    /// @param quadrant the quadrant of the ellipse to draw (see bresenham algorithm)
    /// @param c the filling color
    void ellipse(uint8_t x_center, uint8_t y_center, uint8_t x_radius, uint8_t y_radius, bool fill, uint8_t quadrant, PixelColor c);

protected:
    /// @brief the display device where the attached to the frame buffer
    GraphicDisplayDevice *graphic_display_screen{nullptr};

public:
    /// @brief the dtat structure that contains the actual pixel buffer, created by the display device.
    struct_PixelMemory pixel_memory;
    /// @brief the foregroung color of the graphic frame
    PixelColor fg_color;
    /// @brief the background color of the graphic frame
    PixelColor bg_color;

    /**
     * @brief Construct a new Graphic Framebuffer object
     *
     * @param display_device A pointer to the display device in charge of writing effective pixel in the pixel_buffer
     * @param graph_cfg the graphic configuration structure
     * \image html framebuffer.png
     */
    GraphicFramebuffer(GraphicDisplayDevice *display_device,
                       struct_ConfigGraphicFramebuffer graph_cfg);

    /**
     * @brief Construct a new Graphic Framebuffer object, as the base class of TextualFramebuffer
     *
     * @param display_device A pointer to the display device in charge of writing effective pixel in the pixel_buffer
     * @param frame_width the frame width in pixel
     * @param frame_height the frame height in pixel
     * @param text_cfg the text configuration structure
     */
    GraphicFramebuffer(GraphicDisplayDevice *display_device,
                       size_t frame_width,
                       size_t frame_height,
                       struct_ConfigTextFramebuffer text_cfg);

    /**
     * @brief Construct a new Graphic Framebuffer object.
     * @param display_device A pointer to the display device in charge of writing effective pixel in the pixel_buffer
     * @param text_cfg the configuration file of the textual frame
     */
    GraphicFramebuffer(GraphicDisplayDevice *display_device,
                       struct_ConfigTextFramebuffer text_cfg);

    /**
     * @brief Destroy the GraphicFramebuffer object
     */
    ~GraphicFramebuffer();

    /// @brief Write all pixel buffer memory with "0" (or "1") if color c is BLACK (resp. WHITE)
    /// \note: Works only for monochrome display!
    /// @param pixel_memory the location of the pixel_buffer
    /// @param c the foreground color
    void fill(struct_PixelMemory *pixel_memory, PixelColor c);

    /**
     * @brief  Draw a c color horizontal line, starting at frame position (x,y), on w number of pixel.
     *
     * @param x   horizontal start of line
     * @param y   vertical start of line
     * @param w   length of the line in number of pixel
     * @param c   color of the line, default to WHITE
     */
    void hline(uint8_t x, uint8_t y, size_t w, PixelColor c = PixelColor::WHITE);
    /**
     * @brief  Draw a c color vertical line, starting at frame position (x,y), on w number of pixel.
     *
     * @param x   horizontal start of line
     * @param y   vertical start of line
     * @param h   length of the line in number of pixel
     * @param c   color of the line, default to WHITE
     */
    void vline(uint8_t x, uint8_t y, size_t h, PixelColor c = PixelColor::WHITE);
    /**
     * @brief   Draw a c color line, starting at frame position (x1,y1), ending at frame position (x2,y2)
     *
     * @param x1   horizontal start of line
     * @param y1   vertical start of line
     * @param x2   horizontal end of line
     * @param y2   vertical end of line
     * @param c   color of the line, default to WHITE
     */
    void line(int x1, int y1, int x2, int y2, PixelColor c = PixelColor::WHITE);
    /**
     * @brief   Draw a rectangle, starting at frame position (x,y), w wide and h high
     *
     * @param x   horizontal start of the rectangle
     * @param y   vertical start of the rectangle
     * @param w   number of pixel of the rectangle width
     * @param h   number of pixel of the rectangle height
     * @param fill if true, the rectangle is filled with color c
     * @param c color of the border of the rectangle, default to WHITE
     */
    void rect(uint8_t x, uint8_t y, size_t w, size_t h, bool fill = false, PixelColor c = PixelColor::WHITE);
    /**
     * @brief draw a cercle of size radius, centered at (x_center, y_center)
     * https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_d%27arc_de_cercle_de_Bresenham
     * https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
     * procédure tracerCercle (entier rayon, entier x_centre, entier y_centre)
     *     déclarer entier x, y, m ;
     *    x ← 0 ;
     *    y ← rayon ;             // on se place en haut du cercle
     *    m ← 5 - 4*rayon ;       // initialisation
     *    Tant que x <= y         // tant qu'on est dans le second octant
     *        tracerPixel( x+x_centre, y+y_centre ) ;
     *        tracerPixel( y+x_centre, x+y_centre ) ;
     *        tracerPixel( -x+x_centre, y+y_centre ) ;
     *        tracerPixel( -y+x_centre, x+y_centre ) ;
     *        tracerPixel( x+x_centre, -y+y_centre ) ;
     *        tracerPixel( y+x_centre, -x+y_centre ) ;
     *        tracerPixel( -x+x_centre, -y+y_centre ) ;
     *        tracerPixel( -y+x_centre, -x+y_centre ) ;
     *        si m > 0 alors	//choix du point F
     *            y ← y - 1 ;
     *            m ← m - 8*y ;
     *        fin si ;
     *        x ← x + 1 ;
     *        m ← m + 8*x + 4 ;
     *     fin tant que ;
     * fin de procédure ;
     *
     *
     * @param radius   radius, in pixel, of the circle
     * @param x_center   horizontal position of the center of the cercle
     * @param y_center   vertical position of the center on the cercle
     * @param fill   if true, the circle is filled with color c
     * @param c   color of the border of the circle, default to WHITE
     */
    void circle(int radius, int x_center, int y_center, bool fill = false, PixelColor c = PixelColor::WHITE);
};

/**
 * @brief the place where all textual primitive are placed
 *
 */
class TextualFrameBuffer : public GraphicFramebuffer
{
private:
    /// @brief the line number where the next character will be written.
    uint8_t current_char_line{0};

    /// @brief the column where the next character will be written.
    uint8_t current_char_column{0};

    /// @brief a graphic primitive to draw a character at a character position
    /// @param character the foreground color of the character. The font is given by the frame_text_config
    /// @param char_column the column position of the character
    /// @param char_line the line position of the character
    void drawChar(char character, uint8_t char_column, uint8_t char_line);

    /// @brief clean th full current line (writing " " in the text buffer)
    void clear_line();

protected:
    /// @brief create text buffer and delete if already existing
    void create_text_buffer();

public:
    /// @brief the configuration of the text buffer
    struct_ConfigTextFramebuffer frame_text_config{};
    /// @brief size of the buffer that contains text as string of characters.
    size_t text_buffer_size;
    /// @brief the buffer where text are written
    char *text_buffer = nullptr;
    /// @brief The max number of line with respect to frame height and font height

    /**
     * @brief Construct a new Textual Frame Buffer object
     *
     * @param device A pointer to the display device in charge of showing character
     * @param text_cfg textual configuration data structure
     */
    TextualFrameBuffer(GraphicDisplayDevice *device,
                       struct_ConfigTextFramebuffer text_cfg);

    /**
     * @brief Construct a new Textual Frame Buffer object when the frame size in x and y pixel is given.
     * The number of character line and column are computed according to the size of font
     *
     * @param frame_width the width in pixel of the frame
     * @param frame_height the height in pixel of the frame
     * @param device A pointer to the display device in charge of showing character
     * @param text_cfg the textual configuration data structure
     */
    TextualFrameBuffer(GraphicDisplayDevice *device,
                       size_t frame_width,
                       size_t frame_height,
                       struct_ConfigTextFramebuffer text_cfg);

    ~TextualFrameBuffer();

    /**
     * @brief   Initialize the textual features of framebuffer, according to the configuration data structure frame_text_config
     *
     * @param   frame_text_config the textual configuration data structure
     */
    void
    update_text_buffer(struct_ConfigTextFramebuffer frame_text_config);
    /**
     * @brief   Set text buffer memory to "0" and set character line and column to 0
     */
    void clear_text_buffer();
    /**
     * @brief Update the reference to the font, recompute max number of line, column, the new text buffer size, delete the previous one if any and create a new buffer.
     *
     * @param font
     */
    void update_text_buffer_size(const unsigned char *font);

    /**
     * @brief Update the reference to the font, recompute graphic pixel width and height and the corresponding buffer size, delete the previous one if any and create a new buffer.
     *
     * @param font
     */
    void update_pixel_area(const unsigned char *font); 

    /**
     * @brief convert the internal text buffer characters to the pixel buffer.
     */
    void print_text_buffer();
    /**
     * @brief copy the string c_str to the the internal text buffer, then convert it to the pixel buffer.
     *
     * @param c_str A C_style character string.
     */
    void print_text(const char *c_str); // TODO a remplacer par un draw
    /**
     * @brief convert the c character to the current line and column character position.
     *
     * Text wrapping is done if wrap flag is true.
     * Character position steps forward according to auto_next_char flag.
     *
     * Some special characters are processed:
     *
     *  - "LINE_FEED"       (\\n 0x0A) : line position steps forward, column position is set to 0.
     *
     *  - "BACKSPACE"       (\\b  0x08) : column position steps backward, a space (" ") character is overwritten.
     *
     *  - "FORM_FEED"       (\\f  0x0C) : the text buffer is cleared.
     *
     *  - "CARRIAGE_RETURN" (\\r  0x0D) : column position is set to 0.
     *
     *  - "HORIZONTAL_TAB"  (\\t  0x09) : " " characters are added according to tab_size configuration value.
     * @param c
     */
    void print_char(char c);
    /**
     * @brief character line position steps forward
     */
    void next_line();
    /**
     * @brief character column position steps forward
     */
    void next_char();
};
