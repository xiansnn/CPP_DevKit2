/**
 * @file widget.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "display_device.h"
#include "ui_core.h"

#include <vector>
#include <string>


#include "pico/stdlib.h"
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


class Model;

/**
 * @brief A widget is a displayed object on a device screen. It inherits from all framebuffer features, giving it textual and graphical capabilities.
 *
 * Being a framebuffer, it is defined by a width and a height, line and column of text, and graphics.
 * It is located within the display device screen at an anchor point (x,y).
 *
 * IMPORTANT NOTICE 1:  The widget is effectively drawn if something has changed in the UIModelObejct it represents. This allows to save drawing processing time.
 * However there is a strong limitation : only the widget buffer is transered to the device GDDRAM, based of its specific addressing scheme.
 * As a result, if the widget is located such that the buffer is written across device pages, the contents of the overwritten pages is lost.
 * This is why the widget height and the widget_anchor_y must be multiple of 8. Doing so the widget buffer bytes do not ovewrite pixel outside the widget border.
 *
 * IMPORTANT NOTICE 2: The final widget implementation must know what actual model object it displays. This final implementation must have a member (can be private)
 * e.g. [final_type] * actual_displayed_model; of the actual [final_type] implementation of Model. This can be initialised by the constructor.
 *
 */
class GraphicWidget
{
private:
    /// @brief store the value of the previous blinking phase.should be 0 or 1.
    int8_t previous_blinking_phase;

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

    /// @brief a pointer to the Model actually displayed by the widget
    Model *actual_displayed_model = nullptr;

    virtual void get_value_of_interest() = 0;

    /// @brief ask if the blinking phase has changed
    /// \return true if phase has changed
    bool blinking_phase_has_changed();

    /// @brief The period of the blinking, in microseconds
    uint32_t blink_period_us;

    /// @brief A widget can be composed by several widget.
    std::vector<GraphicWidget *> widgets;

    /// @brief if true, the widget is surrounded by a one-pixel border
    bool widget_with_border{false};

    /// @brief As a widget can be surrounded by a border, the actual widget width is not the associated framebuffer width.
    size_t widget_width{128};

    /// @brief As a widget can be surrounded by a border, the actual widget height is not the associated framebuffer height.
    size_t widget_height{8};
    /**
     * @brief this is the actual horizontal start of the widget drawing area, taken into account the presence of border.
     *
     * WARNING: when the FramebufferFormat format is MONO_VLSB, works fine only if widget_height is a multiple of 8.
     */
    uint8_t widget_start_x;
    /**
     * @brief this is the actual vertical start of the widget drawing area, taken into account the presence of border.
     *
     * WARNING: when the FramebufferFormat format is MONO_VLSB, works fine only if widget_start_y is a multiple of 8
     */
    uint8_t widget_start_y;

    /// @brief this is the border size of the widget. 0 if no border, 1 if border
    uint8_t widget_border_width;

public:
    /// @brief the data structure that contains the actual pixel buffer, created by the display device.
    struct_PixelFrame pixel_frame;
    /// @brief the foregroung color of the graphic frame
    PixelColor fg_color;
    /// @brief the background color of the graphic frame
    PixelColor bg_color;

    /// @brief location in x of the widget within the hosting framebuffer
    uint8_t widget_anchor_x;

    /// @brief location in y of the widget within the hosting framebuffer
    uint8_t widget_anchor_y;

    /**
     * @brief Set the blink period in microseconds
     *
     * @param blink_period default to 1 second
     */
    void set_blink_us(uint32_t blink_period = 1000000);
    /**
     * @brief  add sub_widget to the current widget
     *
     * @param _sub_widget
     */
    void add_widget(GraphicWidget *_sub_widget);

    /// @brief a pure virtual member that is called by draw_refresh method
    virtual void draw() = 0;

    /// @brief draw a rectangle around the widget.
    ///  \note As the border is a rectangle with fill=false, the border width can only be 1 pixel.
    /// @param color the color of the border

    virtual void draw_border(PixelColor color = PixelColor::WHITE);

    /**
     * @brief A short way to call GraphicDisplayDevice::show(pixel_buffer, anchor x, anchor y)
     *
     */
    void show();

    /**
     * @brief Construct a new Graphic Widget object
     * USAGE: when we need a pure graphic widget defined by the struct_ConfigGraphicFramebuffer
     *
     * @param graphic_display_screen The display device on which the widget is drawn.
     * @param displayed_object the displayed object of the widget
     * @param graph_cfg the configuration data structure of the graphic framebuffer
     * @param widget_anchor_x the horizontal position where the widget start on the device screen
     * @param widget_anchor_y the vertical position where the widget start on the device screen
     * @param widget_with_borderThe flag that indicates whether the widget has a border or not
     * \image html widget.png
     */
    GraphicWidget(GraphicDisplayDevice *graphic_display_screen,
                  Model *displayed_object,
                  struct_ConfigGraphicFramebuffer graph_cfg,
                  uint8_t widget_anchor_x,
                  uint8_t widget_anchor_y,
                  bool widget_with_border);
    /**
     * @brief Construct a new Graphic Widget object from the TextWidget Constructor
     * USAGE: When we need a textual framebuffer defined by the struct_ConfigTextFramebuffer
     *
     * @param graphic_display_screen The display device on which the widget is drawn.
     * @param displayed_object the displayed object of the widget
     * @param text_cfg the configuration data structure of the text framebuffer
     * @param widget_anchor_x the horizontal position where the widget start on the device screen
     * @param widget_anchor_y the vertical position where the widget start on the device screen
     * @param widget_with_border flag that indicates whether the widget has a border or not
     */
    GraphicWidget(GraphicDisplayDevice *graphic_display_screen,
                  Model *displayed_object,
                  struct_ConfigTextFramebuffer text_cfg,
                  uint8_t widget_anchor_x,
                  uint8_t widget_anchor_y,
                  bool widget_with_border);

    /**
     * @brief Construct a new Graphic Widget object from the TextWidget Constructor.
     * USAGE: When we need a textual framebuffer defined by the pixel size frame_width, frame_height.
     * The number of column and line are computed with regard to the size of the font
     *
     * @param graphic_display_screen The display device on which the widget is drawn.
     * @param displayed_object the displayed object of the widget
     * @param text_cfg the configuration data structure of the text framebuffer
     * @param frame_width the frame width in pixel
     * @param frame_height the frame height in pixel
     * @param widget_anchor_x the horizontal position where the widget start on the device screen
     * @param widget_anchor_y the vertical position where the widget start on the device screen
     * @param widget_with_border flag that indicates whether the widget has a border or not
     */
    GraphicWidget(GraphicDisplayDevice *graphic_display_screen,
                  Model *displayed_object,
                  struct_ConfigTextFramebuffer text_cfg,
                  size_t frame_width, size_t frame_height,
                  uint8_t widget_anchor_x,
                  uint8_t widget_anchor_y,
                  bool widget_with_border);

    /**
     * @brief Destroy the Widget object
     */
    ~GraphicWidget();

    /**
     * @brief Set the display screen object
     *
     * @param _new_display_device
     */
    void set_display_screen(GraphicDisplayDevice *_new_display_device);

    /**
     * @brief Get the graphic frame config object
     *
     * @return struct_ConfigGraphicFramebuffer
     */
    struct_ConfigGraphicFramebuffer get_graph_frame_config();

    /// @brief Write all pixel buffer memory with "0" (or "1") if color c is BLACK (resp. WHITE)
    /// \note: Works only for monochrome display!
    /// @param c the foreground color
    void fill(PixelColor c);

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
     * @param x0   horizontal start of line
     * @param y0   vertical start of line
     * @param x1   horizontal end of line
     * @param y1   vertical end of line
     * @param c   color of the line, default to WHITE
     */
    void line(int x0, int y0, int x1, int y1, PixelColor c = PixelColor::WHITE);
    /**
     * @brief   Draw a rectangle, starting at frame position (x,y), w wide and h high
     *
     * @param start_x   horizontal start of the rectangle
     * @param start_y   vertical start of the rectangle
     * @param w   number of pixel of the rectangle width
     * @param h   number of pixel of the rectangle height
     * @param fill if true, the rectangle is filled with color c
     * @param c color of the border of the rectangle, default to WHITE
     */
    void rect(uint8_t start_x, uint8_t start_y, size_t w, size_t h, bool fill = false, PixelColor c = PixelColor::WHITE);
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
 * @brief a dedicated class for text frame only
 *
 */
class TextWidget : public GraphicWidget
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
    void write(char character, uint8_t char_column, uint8_t char_line);

    /// @brief clean th full current line (writing " " in the text buffer)
    void clear_line();

    /**
     * @brief The font used. Current font are defined according to IBM CP437.
     *
     * The font files are derived from https://github.com/Harbys/pico-ssd1306 works.
     * They come is size 5x8, 8x8, 12x16 and 16x32.
     */
    const unsigned char *font{nullptr};
    /**
     * @brief  The number of space that ASCII character HT (aka TAB , "\t", 0x9) generates, default to 2
     */
    uint8_t tab_size{2};
    /**
     * @brief Wrap flag : if true, text wrap to the next line when end of line is reached.
     */
    bool wrap{true};
    /**
     * @brief auto_next_char flag : if true each char steps one position after being written.
     */
    bool auto_next_char{true};

protected:
    /// @brief create text buffer and delete the old one if already existing
    void create_text_buffer();

public:
    /// @brief The max number of line with respect to frame height and font height
    uint8_t number_of_column{0};
    /// @brief The max number of column with respect to frame width and font width
    uint8_t number_of_line{0};

    /**
     * @brief Get the text frame config object
     *
     * @return struct_ConfigTextFramebuffer
     */
    struct_ConfigTextFramebuffer get_text_frame_config();

    /// @brief size of the buffer that contains text as string of characters.
    size_t text_buffer_size;
    /// @brief the buffer where text are written
    char *text_buffer = nullptr;
    /// @brief The max number of line with respect to frame height and font height

    /**
     * @brief Construct a new Text Widget object
     *
     * @param device The display device on which the widget is drawn
     * @param text_cfg the configuration data for the textual frame
     * @param displayed_model the displayed model of the widget
     * @param widget_anchor_x the horizontal position where the widget start on the device screen
     * @param widget_anchor_y the verticaThe flag that indicates whether the widget has a border or notl position where the widget start on the device screen
     * @param widget_with_border flag that indicates if the frame has a border
     */
    TextWidget(GraphicDisplayDevice *device,
               struct_ConfigTextFramebuffer text_cfg,
               Model *displayed_model,
               uint8_t widget_anchor_x,
               uint8_t widget_anchor_y,
               bool widget_with_border);

    TextWidget(GraphicDisplayDevice *device,
               size_t frame_width,
               size_t frame_height,
               struct_ConfigTextFramebuffer text_cfg,
               Model *displayed_model,
               uint8_t widget_anchor_x,
               uint8_t widget_anchor_y,
               bool widget_with_border);
    ~TextWidget();

    /**
     * @brief Compute the text size in column x line according to the size of the font and the size of the frame in pixel.
     * Delete the previous text buffer if any and create a new buffer.
     *
     * @param font the new font
     */
    void update_text_frame_size(const unsigned char *font);
    /**
     * @brief   Set text buffer memory to "0" and set  character current line and column to 0
     */
    void clear_text_buffer();

    /**
     * @brief compute graphic pixel width and height according to the size of the text (column x line ) and the size of the bitmap font.
     * Delete the previous pixel buffer if any and create a new buffer.
     *
     * @param font
     */
    void update_graphic_frame_size(const unsigned char *font);

    /**
     * @brief process characters in the internal text buffer and draw it into the pixel buffer.
     */
    void write();
    /**
     * @brief process the string c_str and then draw each character into the pixel buffer, without using the text buffer.
     *
     * @param c_str A C_style character string.
     */
    void write(const char *c_str);
    /**
     * @brief interpret the character and draw it into the pixel buffer at the current line and column character position.
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
     * @param character
     */
    void process_char(char character);
    /**
     * @brief character line steps one position downward.
     */
    void next_line();
    /**
     * @brief character column steps forward one position forward.
     */
    void next_char();

    /**
     * @brief we need draw() to be compliant with the pure virtual draw() inherited from Widget.
     *The draw() member call the show() member automatically.
     * Useful when we mix graphic and text widget in a UI environment.
     * Otherwise, we can use directly the TextFramebuffer::write(), but in this case we have to call draw_border(), then show() member.
     *
     */
    void draw();
    /**
     * @brief draw a one-pixel width around the the frame
     * \note This border can overwrite the characters!
     * To be improve with the use of pixel frame memory not based on byte page such as the OLED SSD1306.
     *
     * @param color
     */
    void draw_border(PixelColor color = PixelColor::WHITE);

    void get_value_of_interest();
};

/**
 * @brief  A widget used when we need to simply print but still want to take advantage of the status change management.
 *
 */
class PrintWidget
{
private:
protected:
    /// @brief a pointer to the Model actually displayed by the widget
    Model *actual_displayed_model = nullptr;
    /// @brief A widget can be composed by several widget.
    std::vector<PrintWidget *> widgets;

public:
    /// @brief a pointer to the printer display device
    PrinterDevice *display_device;

    /**
     * @brief Construct a new Dummy Widget object
     *
     * @param display_device the pointer to the printer display device
     * @param actual_displayed_model the pointer to the displayed model
     */
    PrintWidget(PrinterDevice *display_device, Model *actual_displayed_model);
    ~PrintWidget();

    void add_widget(PrintWidget *widget);

    virtual void print_refresh() = 0;
};
