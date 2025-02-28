/**
 * @file widget.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "widget.h"

#include "pico/stdlib.h"
#include <string.h>
#include <cstring>

void GraphicWidget::draw_border(PixelColor color)
{
    if (this->widget_with_border)
        rect(0, 0, widget_width + 2 * widget_border_width, widget_height + 2 * widget_border_width, false, color);
}

void GraphicWidget::show()
{
    this->graphic_display_screen->show(&this->pixel_frame, this->widget_anchor_x, this->widget_anchor_y);
}

GraphicWidget::GraphicWidget(GraphicDisplayDevice *display_screen,
                             Model *displayed_object,
                             struct_ConfigGraphicWidget graph_cfg)
{
    this->graphic_display_screen = display_screen;

    this->fg_color = graph_cfg.fg_color;
    this->bg_color = graph_cfg.bg_color;

    this->pixel_frame.pixel_frame_height = graph_cfg.pixel_frame_height;
    this->pixel_frame.pixel_frame_width = graph_cfg.pixel_frame_width;
    this->graphic_display_screen->create_pixel_buffer(&this->pixel_frame);

    if (actual_displayed_model != nullptr)
    {
        this->actual_displayed_model = actual_displayed_model;
        this->actual_displayed_model->update_attached_widgets(this);
    }

    this->widget_anchor_x = graph_cfg.widget_anchor_x;
    this->widget_anchor_y = graph_cfg.widget_anchor_y;

    this->widget_with_border = graph_cfg.widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = pixel_frame.pixel_frame_width - 2 * widget_border_width;
    widget_height = pixel_frame.pixel_frame_height - 2 * widget_border_width;

    this->graphic_display_screen->check_display_device_compatibility(get_graph_frame_config());
}

GraphicWidget::GraphicWidget(GraphicDisplayDevice *graphic_display_screen,
                             Model *displayed_object,
                             struct_ConfigTextWidget text_cfg)
{
    this->graphic_display_screen = graphic_display_screen;

    this->fg_color = text_cfg.fg_color;
    this->bg_color = text_cfg.bg_color;

    this->pixel_frame.pixel_frame_width = text_cfg.number_of_column * text_cfg.font[FONT_WIDTH_INDEX];
    this->pixel_frame.pixel_frame_height = text_cfg.number_of_line * text_cfg.font[FONT_HEIGHT_INDEX];
    this->graphic_display_screen->create_pixel_buffer(&this->pixel_frame);

    if (actual_displayed_model != nullptr)
    {
        this->actual_displayed_model = actual_displayed_model;
        this->actual_displayed_model->update_attached_widgets(this);
    }

    this->widget_anchor_x = text_cfg.widget_anchor_x;
    this->widget_anchor_y = text_cfg.widget_anchor_y;

    this->widget_with_border = text_cfg.widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = pixel_frame.pixel_frame_width - 2 * widget_border_width;
    widget_height = pixel_frame.pixel_frame_height - 2 * widget_border_width;

    this->graphic_display_screen->check_display_device_compatibility(get_graph_frame_config());
}

GraphicWidget::GraphicWidget(GraphicDisplayDevice *graphic_display_screen,
                             Model *displayed_object,
                             struct_ConfigTextWidget text_cfg,
                             size_t frame_width, size_t frame_height)
{
    this->graphic_display_screen = graphic_display_screen;

    this->fg_color = text_cfg.fg_color;
    this->bg_color = text_cfg.bg_color;

    this->pixel_frame.pixel_frame_width = frame_width;
    this->pixel_frame.pixel_frame_height = frame_height;

    this->graphic_display_screen->create_pixel_buffer(&this->pixel_frame);

    if (actual_displayed_model != nullptr)
    {
        this->actual_displayed_model = actual_displayed_model;
        this->actual_displayed_model->update_attached_widgets(this);
    }
    this->widget_anchor_x = text_cfg.widget_anchor_x;
    this->widget_anchor_y = text_cfg.widget_anchor_y;

    this->widget_with_border = text_cfg.widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = pixel_frame.pixel_frame_width - 2 * widget_border_width;
    widget_height = pixel_frame.pixel_frame_height - 2 * widget_border_width;

    this->graphic_display_screen->check_display_device_compatibility(get_graph_frame_config());
}

GraphicWidget::~GraphicWidget()
{
    delete[] this->pixel_frame.pixel_frame_buffer;
}

void GraphicWidget::set_display_screen(GraphicDisplayDevice *_new_display_device)
{
    this->graphic_display_screen = _new_display_device;
}

struct_ConfigGraphicWidget GraphicWidget::get_graph_frame_config()
{
    struct_ConfigGraphicWidget cfg = {
        .pixel_frame_width = this->pixel_frame.pixel_frame_width,
        .pixel_frame_height = this->pixel_frame.pixel_frame_height,
        .fg_color = this->fg_color,
        .bg_color = this->bg_color,
        .widget_anchor_x = this->widget_anchor_x,
        .widget_anchor_y = this->widget_anchor_y,
        .widget_with_border = this->widget_with_border};
    return cfg;
}

void GraphicWidget::fill(PixelColor c)
{
    if (c == PixelColor::BLACK)
        memset(this->pixel_frame.pixel_frame_buffer, 0x00, this->pixel_frame.pixel_frame_buffer_size);
    else
        memset(this->pixel_frame.pixel_frame_buffer, 0xFF, this->pixel_frame.pixel_frame_buffer_size);
}

void GraphicWidget::hline(uint8_t x, uint8_t y, size_t w, PixelColor c)
{
    for (size_t i = 0; i < w; i++)
        ((GraphicDisplayDevice *)graphic_display_screen)->pixel(&this->pixel_frame, x + i, y, c);
}

void GraphicWidget::vline(uint8_t x, uint8_t y, size_t h, PixelColor c)
{
    for (size_t i = 0; i < h; i++)
        graphic_display_screen->pixel(&this->pixel_frame, x, y + i, c);
}

void GraphicWidget::line(int x0, int y0, int x1, int y1, PixelColor c)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true)
    {
        graphic_display_screen->pixel(&this->pixel_frame, x0, y0, c);
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

void GraphicWidget::rect(uint8_t start_x, uint8_t start_y, size_t w, size_t h, bool fill, PixelColor c)
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
                graphic_display_screen->pixel(&this->pixel_frame, start_x + i_x, start_y + i_y, c);
}

void GraphicWidget::circle(int radius, int x_center, int y_center, bool fill, PixelColor c)
{
    int x, y, m;
    x = 0;
    y = radius;
    m = 5 - 4 * radius;
    while (x <= y)
    {
        if (!fill)
        {
            graphic_display_screen->pixel(&this->pixel_frame, x_center + x, y_center + y, c);
            graphic_display_screen->pixel(&this->pixel_frame, x_center + y, y_center + x, c);
            graphic_display_screen->pixel(&this->pixel_frame, x_center - x, y_center + y, c);
            graphic_display_screen->pixel(&this->pixel_frame, x_center - y, y_center + x, c);
            graphic_display_screen->pixel(&this->pixel_frame, x_center + x, y_center - y, c);
            graphic_display_screen->pixel(&this->pixel_frame, x_center + y, y_center - x, c);
            graphic_display_screen->pixel(&this->pixel_frame, x_center - x, y_center - y, c);
            graphic_display_screen->pixel(&this->pixel_frame, x_center - y, y_center - x, c);
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

PrintWidget::PrintWidget(PrinterDevice *display_device, Model *actual_displayed_model)
{
    this->display_device = display_device;
    this->actual_displayed_model = actual_displayed_model;
}

PrintWidget::~PrintWidget()
{
}

void PrintWidget::add_widget(PrintWidget *widget)
{
    this->widgets.push_back(widget);
}

void TextWidget::write(char character, uint8_t char_column, uint8_t char_line)
{
    uint8_t anchor_x = char_column * this->font[FONT_WIDTH_INDEX];
    uint8_t anchor_y = char_line * this->font[FONT_HEIGHT_INDEX];
    graphic_display_screen->draw_char_into_pixel(&this->pixel_frame, this->get_text_frame_config(), character, anchor_x, anchor_y);
}

void TextWidget::clear_line()
{
    for (uint8_t i = 0; i < number_of_column; i++)
    {
        write(' ', i, current_char_line);
    }
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
        .fg_color = this->fg_color,
        .bg_color = this->bg_color,
        .wrap = this->wrap,
        .auto_next_char = this->auto_next_char};
    return conf;
}

TextWidget::TextWidget(GraphicDisplayDevice *graphic_display_screen,
                       struct_ConfigTextWidget text_cfg,
                       Model *displayed_object)
    : GraphicWidget(graphic_display_screen, displayed_object, text_cfg)
{
    this->number_of_column = text_cfg.number_of_column;
    this->number_of_line = text_cfg.number_of_line;
    this->font = text_cfg.font;
    this->tab_size = text_cfg.tab_size;
    this->fg_color = text_cfg.fg_color;
    this->bg_color = text_cfg.bg_color;
    this->wrap = text_cfg.wrap;
    this->auto_next_char = text_cfg.auto_next_char;

    create_text_buffer();
}

TextWidget::TextWidget(GraphicDisplayDevice *graphic_display_screen,
                       struct_ConfigTextWidget text_cfg,
                       size_t frame_width, size_t frame_height,
                       Model *displayed_object)
    : GraphicWidget(graphic_display_screen, displayed_object, text_cfg, frame_width, frame_height)
{
    this->font = text_cfg.font;
    this->number_of_column = this->pixel_frame.pixel_frame_width / this->font[FONT_WIDTH_INDEX];
    this->number_of_line = this->pixel_frame.pixel_frame_height / this->font[FONT_HEIGHT_INDEX];

    this->tab_size = text_cfg.tab_size;
    this->fg_color = text_cfg.fg_color;
    this->bg_color = text_cfg.bg_color;
    this->wrap = text_cfg.wrap;
    this->auto_next_char = text_cfg.auto_next_char;

    create_text_buffer();
}

TextWidget::~TextWidget()
{
    delete[] this->text_buffer;
}

void TextWidget::update_text_frame_size(const unsigned char *font)
{
    this->font = font;

    // size the text area according to the available room within the frame whidth and height
    this->number_of_line = this->pixel_frame.pixel_frame_height / font[FONT_HEIGHT_INDEX];
    this->number_of_column = this->pixel_frame.pixel_frame_width / font[FONT_WIDTH_INDEX];

    delete[] this->text_buffer;
    create_text_buffer();
}

void TextWidget::clear_text_buffer()
{
    memset(this->text_buffer, '\0', this->text_buffer_size);
    this->graphic_display_screen->clear_pixel_buffer(&this->pixel_frame);

    current_char_column = 0;
    current_char_line = 0;
}

void TextWidget::update_graphic_frame_size(const unsigned char *font)
{
    this->font = font;
    // size the pixel buffer to the required size due to character area
    this->pixel_frame.pixel_frame_height = number_of_line * font[FONT_HEIGHT_INDEX];
    this->pixel_frame.pixel_frame_width = number_of_column * font[FONT_WIDTH_INDEX];

    delete[] this->pixel_frame.pixel_frame_buffer;
    graphic_display_screen->create_pixel_buffer(&this->pixel_frame);
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
        graphic_display_screen->clear_pixel_buffer(&this->pixel_frame);
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

void TextWidget::draw()
{
    get_value_of_interest();
    write();
    draw_border();
    show();
}

void TextWidget::draw_border(PixelColor color)
{
    if (this->widget_with_border)
        rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, false, color);
}

void TextWidget::get_value_of_interest()
{
}

void GraphicWidget::ellipse(uint8_t x_center, uint8_t y_center, uint8_t x_radius, uint8_t y_radius, bool fill, uint8_t quadrant, PixelColor c)
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
            graphic_display_screen->pixel(&this->pixel_frame, x_center + x, y_center + y, c);
            graphic_display_screen->pixel(&this->pixel_frame, x_center - x, y_center + y, c);
            graphic_display_screen->pixel(&this->pixel_frame, x_center + x, y_center - y, c);
            graphic_display_screen->pixel(&this->pixel_frame, x_center - x, y_center - y, c);
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

bool GraphicWidget::blinking_phase_has_changed()
{
    int8_t current_blinking_phase = (time_us_32() / (this->blink_period_us / 2)) % 2;
    bool phase_has_changed = (previous_blinking_phase != current_blinking_phase);
    previous_blinking_phase = current_blinking_phase;
    return phase_has_changed;
}

void GraphicWidget::update_widget_anchor(uint8_t x, uint8_t y)
{
    this->widget_anchor_x = x;
    this->widget_anchor_y = y;
}

void GraphicWidget::set_blink_us(uint32_t new_blink_period)
{
    this->blink_period_us = new_blink_period;
}

void GraphicWidget::add_widget(GraphicWidget *_sub_widget)
{
    this->widgets.push_back(_sub_widget);
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
