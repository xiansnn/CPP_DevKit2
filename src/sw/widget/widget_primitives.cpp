#include "widget_primitives.h"
#include <cstdlib>

void GraphicDrawer::clear_widget()
{
    canvas->clear_canvas_buffer();
}

GraphicDrawer::GraphicDrawer(struct_ConfigGraphicWidget graph_cfg, CanvasFormat canvas_format)
{
    switch (canvas_format)
    {
    case CanvasFormat::MONO_VLSB:
        this->canvas = new CanvasVLSB(graph_cfg.canvas_width_pixel, graph_cfg.canvas_height_pixel);
        break;
    case CanvasFormat::RGB_COLOR_INDEX_8b:
        this->canvas = new CanvasRGB(graph_cfg.canvas_width_pixel, graph_cfg.canvas_height_pixel);
        break;
    case CanvasFormat::RGB565_16b:
        this->canvas = new CanvasTrueRGB(graph_cfg.canvas_width_pixel, graph_cfg.canvas_height_pixel);
        break;
    default:
        break;
    }
    canvas->fg_color = graph_cfg.canvas_foreground_color;
    canvas->bg_color = graph_cfg.canvas_background_color;

    this->widget_with_border = graph_cfg.widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = canvas->canvas_width_pixel - 2 * widget_border_width;
    widget_height = canvas->canvas_height_pixel - 2 * widget_border_width;
}

GraphicDrawer::GraphicDrawer(struct_ConfigTextWidget text_cfg, CanvasFormat canvas_format)
{
    switch (canvas_format)
    {
        uint8_t canvas_width;
        uint8_t canvas_height;
    case CanvasFormat::MONO_VLSB:
        canvas_width = text_cfg.number_of_column * text_cfg.font[FONT_WIDTH_INDEX];
        canvas_height = text_cfg.number_of_line * text_cfg.font[FONT_HEIGHT_INDEX];
        this->canvas = new CanvasVLSB(canvas_width, canvas_height);
        break;
    case CanvasFormat::RGB_COLOR_INDEX_8b:
        canvas_width = text_cfg.number_of_column * text_cfg.font[FONT_WIDTH_INDEX];
        canvas_height = text_cfg.number_of_line * text_cfg.font[FONT_HEIGHT_INDEX];
        this->canvas = new CanvasRGB(canvas_width, canvas_height);
        break;
    case CanvasFormat::RGB565_16b:
        canvas_width = text_cfg.number_of_column * text_cfg.font[FONT_WIDTH_INDEX];
        canvas_height = text_cfg.number_of_line * text_cfg.font[FONT_HEIGHT_INDEX];
        this->canvas = new CanvasTrueRGB(canvas_width, canvas_height);
        break;
    case CanvasFormat::MONO_HMSB:
        canvas_width = ((text_cfg.number_of_column * text_cfg.font[FONT_WIDTH_INDEX]) + BYTE_SIZE - 1) / BYTE_SIZE * BYTE_SIZE;
        canvas_height = text_cfg.number_of_line * text_cfg.font[FONT_HEIGHT_INDEX];
        this->canvas = new CanvasHMSB(canvas_width, canvas_height);
        break;
    default:
        break;
    }
    canvas->fg_color = text_cfg.fg_color;
    canvas->bg_color = text_cfg.bg_color;

    this->widget_with_border = text_cfg.widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = canvas->canvas_width_pixel - 2 * widget_border_width;
    widget_height = canvas->canvas_height_pixel - 2 * widget_border_width;
}

GraphicDrawer::GraphicDrawer(struct_ConfigTextWidget text_cfg,
                             CanvasFormat canvas_format,
                             size_t frame_width, size_t frame_height)
{
    switch (canvas_format)
    {
    case CanvasFormat::MONO_VLSB:
        this->canvas = new CanvasVLSB(frame_width, frame_height);
        break;
    case CanvasFormat::RGB_COLOR_INDEX_8b:
        this->canvas = new CanvasRGB(frame_width, frame_height);
        break;
    case CanvasFormat::RGB565_16b:
        this->canvas = new CanvasTrueRGB(frame_width, frame_height);
        break;
    case CanvasFormat::MONO_HMSB:
        this->canvas = new CanvasHMSB(frame_width, frame_height);
    default:
        break;
    }
    canvas->fg_color = text_cfg.fg_color;
    canvas->bg_color = text_cfg.bg_color;

    this->widget_with_border = text_cfg.widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = canvas->canvas_width_pixel - 2 * widget_border_width;
    widget_height = canvas->canvas_height_pixel - 2 * widget_border_width;
}

GraphicDrawer::~GraphicDrawer()
{
    delete this->canvas;
}

void GraphicDrawer::hline(uint8_t x, uint8_t y, size_t w, ColorIndex color)
{
    for (size_t i = 0; i < w; i++)
        canvas->draw_pixel(x + i, y, color);
}

void GraphicDrawer::vline(uint8_t x, uint8_t y, size_t h, ColorIndex color)
{
    for (size_t i = 0; i < h; i++)
        canvas->draw_pixel(x, y + i, color);
}

void GraphicDrawer::line(int x0, int y0, int x1, int y1, ColorIndex color)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true)
    {
        canvas->draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void GraphicDrawer::rect(uint8_t start_x, uint8_t start_y, size_t w, size_t h, bool fill, ColorIndex color)
{
    if (!fill)
    {
        hline(start_x, start_y, w, color);
        hline(start_x, start_y + h - 1, w, color);
        vline(start_x, start_y, h, color);
        vline(start_x + w - 1, start_y, h, color);
    }
    else
        for (size_t i_x = 0; i_x < w; i_x++)
            for (size_t i_y = 0; i_y < h; i_y++)
                canvas->draw_pixel(start_x + i_x, start_y + i_y, color);
}

void GraphicDrawer::circle(int radius, int x_center, int y_center, bool fill, ColorIndex color)
{
    int x, y, m;
    x = 0;
    y = radius;
    m = 5 - 4 * radius;
    while (x <= y)
    {
        if (!fill)
        {
            canvas->draw_pixel(x_center + x, y_center + y, color);
            canvas->draw_pixel(x_center + y, y_center + x, color);
            canvas->draw_pixel(x_center - x, y_center + y, color);
            canvas->draw_pixel(x_center - y, y_center + x, color);
            canvas->draw_pixel(x_center + x, y_center - y, color);
            canvas->draw_pixel(x_center + y, y_center - x, color);
            canvas->draw_pixel(x_center - x, y_center - y, color);
            canvas->draw_pixel(x_center - y, y_center - x, color);
        }
        else
        {
            hline(x_center - x, y_center + y, 2 * x + 2, color);
            hline(x_center - y, y_center + x, 2 * y + 2, color);
            hline(x_center - y, y_center - x, 2 * y + 2, color);
            hline(x_center - x, y_center - y, 2 * x + 2, color);
        }
        if (m > 0)
        {
            y -= 1;
            m -= 8 * y;
        }
        x += 1;
        m += 8 * x + 4;
    }
}

void GraphicDrawer::draw_border(ColorIndex color)
{
    if (this->widget_with_border)
        rect(0, 0, widget_width + 2 * widget_border_width, widget_height + 2 * widget_border_width, false, color);
}

void TextWriter::write(char character, uint8_t char_column, uint8_t char_line)
{
    uint8_t position_x = char_column * this->font[FONT_WIDTH_INDEX];
    uint8_t position_y = char_line * this->font[FONT_HEIGHT_INDEX];
    draw_glyph(character, position_x, position_y);
}

void TextWriter::clear_line()
{
    for (uint8_t i = 0; i < number_of_column; i++)
        write(' ', i, current_char_line);
}

void TextWriter::draw_glyph(const char character,
                            const uint8_t anchor_x, const uint8_t anchor_y)
{
    if ((font == nullptr) || (character < 32))
        return;

    uint8_t font_width = font[FONT_WIDTH_INDEX];
    uint8_t font_height = font[FONT_HEIGHT_INDEX];

    uint16_t seek = (character - 32) * (font_width * font_height) / 8 + 2;

    uint8_t b_seek = 0;

    for (uint8_t x = 0; x < font_width; x++)
    {
        for (uint8_t y = 0; y < font_height; y++)
        {
            uint8_t b = font[seek];
            if (font[seek] >> b_seek & 0b00000001)
                canvas->draw_pixel(x + anchor_x, y + anchor_y, canvas->fg_color);
            else
                canvas->draw_pixel(x + anchor_x, y + anchor_y, canvas->bg_color);

            b_seek++;
            if (b_seek == 8)
            {
                b_seek = 0;
                seek++;
            }
        }
    }
}

void TextWriter::create_text_buffer()
{
    this->text_buffer_size = number_of_column * number_of_line + 1;
    this->text_buffer = new char[text_buffer_size];
    clear_text_buffer();
}

TextWriter::TextWriter(struct_ConfigTextWidget text_cfg, CanvasFormat canvas_format)
    : GraphicDrawer(text_cfg, canvas_format)
{
    this->font = text_cfg.font;

    this->number_of_column = text_cfg.number_of_column;
    this->number_of_line = text_cfg.number_of_line;

    this->tab_size = text_cfg.tab_size;
    this->wrap = text_cfg.wrap;
    this->auto_next_char = text_cfg.auto_next_char;

    create_text_buffer();
}

TextWriter::TextWriter(struct_ConfigTextWidget text_cfg,
                       CanvasFormat canvas_format,
                       size_t frame_width, size_t frame_height)
    : GraphicDrawer(text_cfg, canvas_format, frame_width, frame_height)
{
    this->font = text_cfg.font;

    this->number_of_column = this->canvas->canvas_width_pixel / this->font[FONT_WIDTH_INDEX];
    this->number_of_line = this->canvas->canvas_height_pixel / this->font[FONT_HEIGHT_INDEX];
    this->canvas->canvas_buffer_size_pixel = this->canvas->canvas_height_pixel * this->canvas->canvas_width_pixel;

    this->tab_size = text_cfg.tab_size;
    this->wrap = text_cfg.wrap;
    this->auto_next_char = text_cfg.auto_next_char;

    create_text_buffer();
}

TextWriter::~TextWriter()
{
    delete[] this->text_buffer;
}

void TextWriter::update_text_line_column_number(const unsigned char *font)
{
    this->font = font;
    // size the text area according to the available room within the frame width and height
    this->number_of_line = this->canvas->canvas_height_pixel / font[FONT_HEIGHT_INDEX];
    this->number_of_column = this->canvas->canvas_width_pixel / font[FONT_WIDTH_INDEX];

    delete[] this->text_buffer;
    create_text_buffer();
}

void TextWriter::clear_text_buffer()
{
    memset(this->text_buffer, '\0', this->text_buffer_size);
    canvas->clear_canvas_buffer();
    current_char_column = 0;
    current_char_line = 0;
}

void TextWriter::update_canvas_buffer_size(const unsigned char *font)
{
    this->font = font;
    // size the pixel buffer to the required size due to character area
    this->canvas->canvas_height_pixel = number_of_line * font[FONT_HEIGHT_INDEX];
    this->canvas->canvas_width_pixel = number_of_column * font[FONT_WIDTH_INDEX];
    this->canvas->canvas_buffer_size_pixel = this->canvas->canvas_height_pixel * this->canvas->canvas_width_pixel;

    if (canvas->canvas_format == CanvasFormat::RGB565_16b)
        delete[] canvas->canvas_16buffer;
    else
        delete[] canvas->canvas_buffer;

    this->canvas->create_canvas_buffer();
}

void TextWriter::write()
{
    write(this->text_buffer);
}

void TextWriter::write(const char *c_str)
{
    uint16_t n = 0;
    while (c_str[n] != '\0')
    {
        process_char(c_str[n]);
        n++;
    }
}

void TextWriter::process_char(char character)
{
    switch (character)
    {
    case VERTICAL_TAB:
        break;
    case LINE_FEED:
        next_line();
        current_char_column = 0;
        break;
    case BACKSPACE:
        current_char_column--;
        write(' ', current_char_column, current_char_line);
        break;
    case FORM_FEED:
        canvas->clear_canvas_buffer();
        current_char_column = 0;
        current_char_line = 0;
        break;
    case CARRIAGE_RETURN:
        current_char_column = 0;
        break;
    default:
        if (current_char_column == 0)
            clear_line(); // start a new line
        if (character == HORIZONTAL_TAB)
        {
            for (uint8_t i = 0; i < tab_size; i++)
            {
                write(' ', current_char_column, current_char_line);
                next_char();
            }
        }
        else
        {
            if (this->auto_next_char)
            {
                write(character, current_char_column, current_char_line);
                next_char();
            }
            else
            {
                write(' ', current_char_column, current_char_line);
                write(character, current_char_column, current_char_line);
            }
        }
        break;
    }
}

void TextWriter::next_line()
{
    current_char_column = 0;
    current_char_line++;
    if (current_char_line >= number_of_line)
        current_char_line = 0;
}

void TextWriter::next_char()
{
    current_char_column++;
    if (current_char_column >= number_of_column)
    {
        if (wrap)
        {
            current_char_column = 0;
            next_line();
        }
    }
}

void TextWriter::draw_border(ColorIndex color)
{
    if (this->widget_with_border)
        rect(0, 0, canvas->canvas_width_pixel, canvas->canvas_height_pixel, false, color);
}

/*   code that we may need :
//================================================================================================
void GraphicFramebuffer::byteOR(int byte_idx, uint8_t byte)
{
    // return if index outside 0 - pixel_buffer length - 1
    if (byte_idx > (this->pixel_buffer_size - 1))
        return;
    this->pixel_buffer[byte_idx] |= byte;
}
void GraphicFramebuffer::byteAND(int byte_idx, uint8_t byte)
{
    // return if index outside 0 - pixel_buffer length - 1
    if (byte_idx > (this->pixel_buffer_size - 1))
        return;
    this->pixel_buffer[byte_idx] &= byte;
}
void GraphicFramebuffer::byteXOR(int byte_idx, uint8_t byte)
{
    // return if index outside 0 - pixel_buffer length - 1
    if (byte_idx > (this->pixel_buffer_size - 1))
        return;
    this->pixel_buffer[byte_idx] ^= byte;
}
//==============================================================================================

    /// @brief the graphic primitive to draw an ellipse \bug //FIXME ellipse doesn't work !
    /// @param x_center the x coordinate of the center
    /// @param y_center the y coordinate of the center
    /// @param x_radius the radius along x axis
    /// @param y_radius the radius along y axis
    /// @param fill a flag that indicates whether the ellipse is filled or not
    /// @param quadrant the quadrant of the ellipse to draw (see bresenham algorithm)
    /// @param color the filling color
void GraphicWidget::ellipse(uint8_t x_center, uint8_t y_center, uint8_t x_radius, uint8_t y_radius, bool fill, uint8_t quadrant, ColorIndex color)
{
    int x, y, m;
    x = 0;
    y = y_radius;
    int _xr2 = x_radius * x_radius;
    int _yr2 = y_radius * y_radius;

    m = 4 * _yr2 - 4 * _xr2 * y_radius + _xr2;

    while (y >= 0)
    {
        if (!fill)
        {
            canvas->draw_pixel(x_center + x, y_center + y, color);
            canvas->draw_pixel(x_center - x, y_center + y, color);
            canvas->draw_pixel(x_center + x, y_center - y, color);
            canvas->draw_pixel(x_center - x, y_center - y, color);
        }
        else
        {
            hline(x_center - x, y_center + y, 2 * x + 2, color);
            hline(x_center - y, y_center + x, 2 * y + 2, color);
            hline(x_center - y, y_center - x, 2 * y + 2, color);
            hline(x_center - x, y_center - y, 2 * x + 2, color);
        }

        if (m > 0)
        {
            x += 1;
            y -= 1;
            m += 3 * _xr2 - 4 * _xr2 * y;
        }
        else
        {
            x += 1;
            m += 4 * _xr2 * y - _xr2;
        }
    }
}

*/
