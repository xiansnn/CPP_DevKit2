
#include "widget.h"

#include "pico/stdlib.h"
#include <string.h>
#include <cstring>

// #define PRINT_WIDGET_CONSTRUCTOR_DESTRUCTOR  //usefull to debug widget construction/destruction

Widget::Widget(Model *actual_displayed_model, DisplayDevice *graphic_display_device)
{
#if defined(PRINT_WIDGET_CONSTRUCTOR_DESTRUCTOR)
    printf("+ Widget\n");
#endif // MACRO
    this->display_device = graphic_display_device;
    if (actual_displayed_model != nullptr)
    {
        this->actual_displayed_model = actual_displayed_model;
        this->actual_displayed_model->update_attached_widgets(this);
        actual_displayed_model->set_change_flag();
    }
}

Widget::~Widget()
{
#if defined(PRINT_WIDGET_CONSTRUCTOR_DESTRUCTOR)
    printf("- Widget\n");
#endif // MACRO
}

void Widget::add_widget(Widget *_sub_widget)
{
    this->widgets.push_back(_sub_widget);
}

void Widget::set_display_device(DisplayDevice *_new_display_device)
{
    this->display_device = _new_display_device;
}

void GraphicWidget::draw_border(ColorIndex color)
{
    if (this->widget_with_border)
        rect(0, 0, widget_width + 2 * widget_border_width, widget_height + 2 * widget_border_width, false, color);
}

void GraphicWidget::show()
{
    ((GraphicDisplayDevice *)display_device)->show(this->canvas, this->widget_anchor_x, this->widget_anchor_y);
}

void GraphicWidget::send_to_DisplayGateKeeper(QueueHandle_t display_queue, SemaphoreHandle_t sending_done)
{
    xQueueSend(display_queue, &(this->data_to_display), portMAX_DELAY); // take 65ms but used fully the CPU
    xSemaphoreTake(sending_done, portMAX_DELAY);

}

GraphicWidget::GraphicWidget(GraphicDisplayDevice *graphic_display_screen,
                             struct_ConfigGraphicWidget graph_cfg,
                             CanvasFormat canvas_format,
                             Model *displayed_object)
    : Widget(displayed_object, graphic_display_screen)
{
#if defined(PRINT_WIDGET_CONSTRUCTOR_DESTRUCTOR)
    printf("+ GraphicWidget graph_cfg\n");
#endif // MACRO
    switch (canvas_format)
    {
    case CanvasFormat::MONO_VLSB:
        this->canvas = new CanvasVLSB(graph_cfg.pixel_frame_width, graph_cfg.pixel_frame_height);
        break;
    case CanvasFormat::RGB_COLOR_INDEX_8b:
        this->canvas = new CanvasRGB(graph_cfg.pixel_frame_width, graph_cfg.pixel_frame_height);
        break;
    case CanvasFormat::RGB565_16b:
        this->canvas = new CanvasTrueRGB(graph_cfg.pixel_frame_width, graph_cfg.pixel_frame_height);
        break;
    default:
        break;
    }
    canvas->canvas_fg_color = graph_cfg.fg_color;
    canvas->canvas_bg_color = graph_cfg.bg_color;

    this->widget_anchor_x = graph_cfg.widget_anchor_x;
    this->widget_anchor_y = graph_cfg.widget_anchor_y;

    this->widget_with_border = graph_cfg.widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = canvas->canvas_width_pixel - 2 * widget_border_width;
    widget_height = canvas->canvas_height_pixel - 2 * widget_border_width;

    data_to_display.display = (GraphicDisplayDevice *)display_device;
    data_to_display.canvas = this->canvas;
    data_to_display.anchor_x = this->widget_anchor_x;
    data_to_display.anchor_y = this->widget_anchor_y;

    ((GraphicDisplayDevice *)display_device)->check_display_device_compatibility(get_graph_frame_config());
}

GraphicWidget::GraphicWidget(GraphicDisplayDevice *graphic_display_screen,
                             struct_ConfigTextWidget text_cfg,
                             CanvasFormat canvas_format,
                             Model *displayed_object)
    : Widget(displayed_object, graphic_display_screen)
{
#if defined(PRINT_WIDGET_CONSTRUCTOR_DESTRUCTOR)
    printf("+ GraphicWidget text_cfg\n");
#endif // MACRO

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
    canvas->canvas_fg_color = text_cfg.fg_color;
    canvas->canvas_bg_color = text_cfg.bg_color;

    this->widget_anchor_x = text_cfg.widget_anchor_x;
    this->widget_anchor_y = text_cfg.widget_anchor_y;

    this->widget_with_border = text_cfg.widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = canvas->canvas_width_pixel - 2 * widget_border_width;
    widget_height = canvas->canvas_height_pixel - 2 * widget_border_width;

    data_to_display.display = (GraphicDisplayDevice *)display_device;
    data_to_display.canvas = this->canvas;
    data_to_display.anchor_x = this->widget_anchor_x;
    data_to_display.anchor_y = this->widget_anchor_y;

    ((GraphicDisplayDevice *)display_device)->check_display_device_compatibility(get_graph_frame_config());
}

GraphicWidget::GraphicWidget(GraphicDisplayDevice *graphic_display_screen,
                             struct_ConfigTextWidget text_cfg,
                             CanvasFormat canvas_format,
                             size_t frame_width, size_t frame_height,
                             Model *displayed_object)
    : Widget(displayed_object, graphic_display_screen)
{
#if defined(PRINT_WIDGET_CONSTRUCTOR_DESTRUCTOR)
    printf("+ GraphicWidget text_cfg 2\n");
#endif // MACRO
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
    canvas->canvas_fg_color = text_cfg.fg_color;
    canvas->canvas_bg_color = text_cfg.bg_color;

    this->widget_anchor_x = text_cfg.widget_anchor_x;
    this->widget_anchor_y = text_cfg.widget_anchor_y;

    this->widget_with_border = text_cfg.widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = canvas->canvas_width_pixel - 2 * widget_border_width;
    widget_height = canvas->canvas_height_pixel - 2 * widget_border_width;

    data_to_display.display = (GraphicDisplayDevice *)display_device;
    data_to_display.canvas = this->canvas;
    data_to_display.anchor_x = this->widget_anchor_x;
    data_to_display.anchor_y = this->widget_anchor_y;

    ((GraphicDisplayDevice *)display_device)->check_display_device_compatibility(get_graph_frame_config());
}

GraphicWidget::~GraphicWidget()
{
#if defined(PRINT_WIDGET_CONSTRUCTOR_DESTRUCTOR)
    printf("- GraphicWidget\n");
#endif // MACRO
    delete canvas;
}

struct_ConfigGraphicWidget GraphicWidget::get_graph_frame_config()
{
    struct_ConfigGraphicWidget cfg = {
        .pixel_frame_width = canvas->canvas_width_pixel,
        .pixel_frame_height = canvas->canvas_height_pixel,
        .fg_color = canvas->canvas_fg_color,
        .bg_color = canvas->canvas_bg_color,
        .widget_anchor_x = this->widget_anchor_x,
        .widget_anchor_y = this->widget_anchor_y,
        .widget_with_border = this->widget_with_border};
    return cfg;
}

void GraphicWidget::hline(uint8_t x, uint8_t y, size_t w, ColorIndex c)
{
    for (size_t i = 0; i < w; i++)
        canvas->draw_pixel(x + i, y, c);
}

void GraphicWidget::vline(uint8_t x, uint8_t y, size_t h, ColorIndex c)
{
    for (size_t i = 0; i < h; i++)
        canvas->draw_pixel(x, y + i, c);
}

void GraphicWidget::line(int x0, int y0, int x1, int y1, ColorIndex c)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true)
    {
        canvas->draw_pixel(x0, y0, c);
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

void GraphicWidget::rect(uint8_t start_x, uint8_t start_y, size_t w, size_t h, bool fill, ColorIndex c)
{
    if (!fill)
    {
        hline(start_x, start_y, w, c);
        hline(start_x, start_y + h - 1, w, c);
        vline(start_x, start_y, h, c);
        vline(start_x + w - 1, start_y, h, c);
    }
    else
        for (size_t i_x = 0; i_x < w; i_x++)
            for (size_t i_y = 0; i_y < h; i_y++)
                canvas->draw_pixel(start_x + i_x, start_y + i_y, c);
}

void GraphicWidget::circle(int radius, int x_center, int y_center, bool fill, ColorIndex c)
{
    int x, y, m;
    x = 0;
    y = radius;
    m = 5 - 4 * radius;
    while (x <= y)
    {
        if (!fill)
        {
            canvas->draw_pixel(x_center + x, y_center + y, c);
            canvas->draw_pixel(x_center + y, y_center + x, c);
            canvas->draw_pixel(x_center - x, y_center + y, c);
            canvas->draw_pixel(x_center - y, y_center + x, c);
            canvas->draw_pixel(x_center + x, y_center - y, c);
            canvas->draw_pixel(x_center + y, y_center - x, c);
            canvas->draw_pixel(x_center - x, y_center - y, c);
            canvas->draw_pixel(x_center - y, y_center - x, c);
        }
        else
        {
            hline(x_center - x, y_center + y, 2 * x + 2, c);
            hline(x_center - y, y_center + x, 2 * y + 2, c);
            hline(x_center - y, y_center - x, 2 * y + 2, c);
            hline(x_center - x, y_center - y, 2 * x + 2, c);
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

void TextWidget::write(char character, uint8_t char_column, uint8_t char_line)
{
    uint8_t anchor_x = char_column * this->font[FONT_WIDTH_INDEX];
    uint8_t anchor_y = char_line * this->font[FONT_HEIGHT_INDEX];
    draw_glyph(character, anchor_x, anchor_y);
}

void TextWidget::clear_line()
{
    for (uint8_t i = 0; i < number_of_column; i++)
        write(' ', i, current_char_line);
}

void TextWidget::create_text_buffer()
{
    this->text_buffer_size = number_of_column * number_of_line + 1;

    this->text_buffer = new char[text_buffer_size];
    clear_text_buffer();
}

struct_ConfigTextWidget TextWidget::get_text_frame_config()
{
    struct_ConfigTextWidget conf = {
        .number_of_column = this->number_of_column,
        .number_of_line = this->number_of_line,
        .widget_anchor_x = this->widget_anchor_x,
        .widget_anchor_y = this->widget_anchor_y,
        .font = this->font,
        .tab_size = this->tab_size,
        .fg_color = this->canvas->canvas_fg_color,
        .bg_color = this->canvas->canvas_bg_color,
        .wrap = this->wrap,
        .auto_next_char = this->auto_next_char};
    return conf;
}

TextWidget::TextWidget(GraphicDisplayDevice *graphic_display_screen,
                       struct_ConfigTextWidget text_cfg, CanvasFormat canvas_format,
                       Model *displayed_object)
    : GraphicWidget(graphic_display_screen, text_cfg, canvas_format, displayed_object)
{
#if defined(PRINT_WIDGET_CONSTRUCTOR_DESTRUCTOR)
    printf("+ TextWidget\n");
#endif // MACRO
    this->number_of_column = text_cfg.number_of_column;
    this->number_of_line = text_cfg.number_of_line;
    this->font = text_cfg.font;
    this->tab_size = text_cfg.tab_size;
    this->wrap = text_cfg.wrap;
    this->auto_next_char = text_cfg.auto_next_char;

    create_text_buffer();
}

TextWidget::TextWidget(GraphicDisplayDevice *graphic_display_screen,
                       struct_ConfigTextWidget text_cfg, CanvasFormat canvas_format,
                       size_t frame_width, size_t frame_height,
                       Model *displayed_object)
    : GraphicWidget(graphic_display_screen, text_cfg, canvas_format, frame_width, frame_height, displayed_object)
{
#if defined(PRINT_WIDGET_CONSTRUCTOR_DESTRUCTOR)
    printf("+ TextWidget2\n");
#endif // MACRO
    this->font = text_cfg.font;
    this->number_of_column = this->canvas->canvas_width_pixel / this->font[FONT_WIDTH_INDEX];
    this->number_of_line = this->canvas->canvas_height_pixel / this->font[FONT_HEIGHT_INDEX];
    this->canvas->canvas_buffer_size_pixel = this->canvas->canvas_height_pixel * this->canvas->canvas_width_pixel;

    this->tab_size = text_cfg.tab_size;

    this->wrap = text_cfg.wrap;
    this->auto_next_char = text_cfg.auto_next_char;

    create_text_buffer();
}

TextWidget::~TextWidget()
{
#if defined(PRINT_WIDGET_CONSTRUCTOR_DESTRUCTOR)
    printf("- TextWidget\n");
#endif // MACRO
    delete[] this->text_buffer;
}

void TextWidget::update_text_frame_size(const unsigned char *font)
{
    this->font = font;

    // size the text area according to the available room within the frame width and height
    this->number_of_line = this->canvas->canvas_height_pixel / font[FONT_HEIGHT_INDEX];
    this->number_of_column = this->canvas->canvas_width_pixel / font[FONT_WIDTH_INDEX];

    delete[] this->text_buffer;
    create_text_buffer();
}

void TextWidget::clear_text_buffer()
{
    memset(this->text_buffer, '\0', this->text_buffer_size);
    canvas->clear_canvas_buffer();
    current_char_column = 0;
    current_char_line = 0;
}

void TextWidget::update_canvas_buffer_size(const unsigned char *font)
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

void TextWidget::write()
{
    write(this->text_buffer);
}

void TextWidget::write(const char *c_str)
{
    uint16_t n = 0;
    while (c_str[n] != '\0')
    {
        process_char(c_str[n]);
        n++;
    }
}

void TextWidget::process_char(char character)
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

void TextWidget::next_line()
{
    current_char_column = 0;
    current_char_line++;
    if (current_char_line >= number_of_line)
        current_char_line = 0;
}

void TextWidget::next_char()
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

void TextWidget::draw_glyph(const char character,
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
                canvas->draw_pixel(x + anchor_x, y + anchor_y, canvas->canvas_fg_color);
            else
                canvas->draw_pixel(x + anchor_x, y + anchor_y, canvas->canvas_bg_color);

            b_seek++;
            if (b_seek == 8)
            {
                b_seek = 0;
                seek++;
            }
        }
    }
}

void TextWidget::draw()
{
    if (this->actual_displayed_model->has_changed())
    {
        clear_text_buffer();
        get_value_of_interest();
        write();
        draw_border();
        show();
        this->actual_displayed_model->draw_widget_done();
    }
}

void TextWidget::draw_border(ColorIndex color)
{
    if (this->widget_with_border)
        rect(0, 0, canvas->canvas_width_pixel, canvas->canvas_height_pixel, false, color);
}

void GraphicWidget::ellipse(uint8_t x_center, uint8_t y_center, uint8_t x_radius, uint8_t y_radius, bool fill, uint8_t quadrant, ColorIndex c)
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
            canvas->draw_pixel(x_center + x, y_center + y, c);
            canvas->draw_pixel(x_center - x, y_center + y, c);
            canvas->draw_pixel(x_center + x, y_center - y, c);
            canvas->draw_pixel(x_center - x, y_center - y, c);
        }
        else
        {
            hline(x_center - x, y_center + y, 2 * x + 2, c);
            hline(x_center - y, y_center + x, 2 * y + 2, c);
            hline(x_center - y, y_center - x, 2 * y + 2, c);
            hline(x_center - x, y_center - y, 2 * x + 2, c);
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

void GraphicWidget::clear_widget()
{
    canvas->clear_canvas_buffer();
}

void GraphicWidget::update_widget_anchor(uint8_t x, uint8_t y)
{
    this->widget_anchor_x = x;
    this->widget_anchor_y = y;
}

// void GraphicFramebuffer::byteOR(int byte_idx, uint8_t byte)
// {
//     // return if index outside 0 - pixel_buffer length - 1
//     if (byte_idx > (this->pixel_buffer_size - 1))
//         return;
//     this->pixel_buffer[byte_idx] |= byte;
// }
// void GraphicFramebuffer::byteAND(int byte_idx, uint8_t byte)
// {
//     // return if index outside 0 - pixel_buffer length - 1
//     if (byte_idx > (this->pixel_buffer_size - 1))
//         return;
//     this->pixel_buffer[byte_idx] &= byte;
// }
// void GraphicFramebuffer::byteXOR(int byte_idx, uint8_t byte)
// {
//     // return if index outside 0 - pixel_buffer length - 1
//     if (byte_idx > (this->pixel_buffer_size - 1))
//         return;
//     this->pixel_buffer[byte_idx] ^= byte;
// }

void Blinker::compute_blinking_phase()
{
    int8_t current_blinking_phase = (time_us_32() / (this->blink_period_us / 2)) % 2;
    blink_phase_changed = (previous_blinking_phase != current_blinking_phase);
    previous_blinking_phase = current_blinking_phase;
}

Blinker::Blinker()
{
}

Blinker::~Blinker()
{
}

void Blinker::set_blink_us(uint32_t blink_period)
{
    this->blink_period_us = blink_period;
}

bool Blinker::has_blinking_changed()
{
    return blink_phase_changed;
}

void Blinker::clear_blinking_phase_change()
{
    blink_phase_changed = false;
}

PrintWidget::PrintWidget(PrinterDevice *display_device, Model *actual_displayed_model)
    : Widget(actual_displayed_model, display_device)
{
}

PrintWidget::~PrintWidget()
{
}
