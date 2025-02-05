/**
 * @file framebuffer.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "framebuffer.h"
#include "pico/stdlib.h"
#include <string.h>
#include <cstring>

GraphicFramebuffer::GraphicFramebuffer(DisplayDevice *device,
                                       //  size_t _frame_width,
                                       //  size_t _frame_height,
                                       //  PixelColor fg_color,
                                       //  PixelColor bg_color
                                       struct_GraphicFramebufferConfig graph_cfg)
{
    this->display_screen = device;
    this->fg_color = fg_color;
    this->bg_color = bg_color;
    this->pixel_memory.frame_height = graph_cfg.frame_height;
    this->pixel_memory.frame_width = graph_cfg.frame_width;
    ((GraphicDisplayDevice *)device)->create_pixel_buffer(&this->pixel_memory);
}

GraphicFramebuffer::GraphicFramebuffer(DisplayDevice *display_device,
                                       size_t frame_width,
                                       size_t frame_height,
                                       struct_TextFramebufferConfig text_cfg)
{
    this->display_screen = display_device;
    this->fg_color = text_cfg.fg_color;
    this->bg_color = text_cfg.bg_color;
    this->pixel_memory.frame_height = frame_height;
    this->pixel_memory.frame_width = frame_width;
    ((GraphicDisplayDevice *)display_device)->create_pixel_buffer(&this->pixel_memory);
}

GraphicFramebuffer::GraphicFramebuffer(GraphicDisplayDevice *device,
                                       //  uint8_t number_of_column, // utilisé pour heritage textualframebuffer
                                       //  uint8_t number_of_line,
                                       struct_TextFramebufferConfig text_cnf)
{
    this->display_screen = device;
    this->fg_color = text_cnf.fg_color;
    this->bg_color = text_cnf.bg_color;

    this->pixel_memory.frame_width = text_cnf.number_of_column * text_cnf.font[FONT_WIDTH_INDEX];
    this->pixel_memory.frame_height = text_cnf.number_of_line * text_cnf.font[FONT_HEIGHT_INDEX]; // TODO verifier qu'on depasse pas la taille de display. il faudrait connaitre le displaydevice !!!
    device->create_pixel_buffer(&this->pixel_memory);
}

GraphicFramebuffer::~GraphicFramebuffer()
{
}

void GraphicFramebuffer::fill(struct_PixelMemory *pixel_memory, PixelColor c)
{
    if (c == PixelColor::BLACK)
        memset(pixel_memory->pixel_buffer, 0x00, pixel_memory->pixel_buffer_size);
    else
        memset(pixel_memory->pixel_buffer, 0xFF, pixel_memory->pixel_buffer_size);
}

void GraphicFramebuffer::hline(uint8_t x, uint8_t y, size_t w, PixelColor c)
{
    for (size_t i = 0; i < w; i++)
        ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x + i, y, c);
}

void GraphicFramebuffer::vline(uint8_t x, uint8_t y, size_t h, PixelColor c)
{
    for (size_t i = 0; i < h; i++)
        ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x, y + i, c);
}

void GraphicFramebuffer::line(int x0, int y0, int x1, int y1, PixelColor c)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true)
    {
        ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x0, y0, c);
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

void GraphicFramebuffer::rect(uint8_t start_x, uint8_t start_y, size_t w, size_t h, bool fill, PixelColor c)
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
                ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, start_x + i_x, start_y + i_y, c);
}

void GraphicFramebuffer::ellipse(uint8_t x_center, uint8_t y_center, uint8_t x_radius, uint8_t y_radius, bool fill, uint8_t quadrant, PixelColor c)
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
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center + x, y_center + y, c);
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center - x, y_center + y, c);
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center + x, y_center - y, c);
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center - x, y_center - y, c);
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

void GraphicFramebuffer::circle(int radius, int x_center, int y_center, bool fill, PixelColor c)
{
    int x, y, m;
    x = 0;
    y = radius;
    m = 5 - 4 * radius;
    while (x <= y)
    {
        if (!fill)
        {
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center + x, y_center + y, c);
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center + y, y_center + x, c);
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center - x, y_center + y, c);
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center - y, y_center + x, c);
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center + x, y_center - y, c);
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center + y, y_center - x, c);
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center - x, y_center - y, c);
            ((GraphicDisplayDevice *)display_screen)->pixel(&this->pixel_memory, x_center - y, y_center - x, c);
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

void TextualFrameBuffer::drawChar(char c, uint8_t char_column, uint8_t char_line)
{
    uint8_t anchor_x = char_column * frame_text_config.font[FONT_WIDTH_INDEX];
    uint8_t anchor_y = char_line * frame_text_config.font[FONT_HEIGHT_INDEX];
    ((GraphicDisplayDevice *)display_screen)->drawChar(&this->pixel_memory, &this->frame_text_config, c, anchor_x, anchor_y);
}

void TextualFrameBuffer::clear_line()
{
    for (uint8_t i = 0; i < this->number_of_column; i++)
    {
        drawChar(' ', i, current_char_line);
    }
}

void TextualFrameBuffer::create_text_buffer()
{
    this->text_buffer_size = number_of_column * number_of_line + 1;

    this->text_buffer = new char[text_buffer_size];
    clear_text_buffer();
}

TextualFrameBuffer::TextualFrameBuffer(GraphicDisplayDevice *device,
                                       //    uint8_t number_of_column,
                                       //    uint8_t number_of_line,
                                       struct_TextFramebufferConfig text_cfg)
    : GraphicFramebuffer(device, text_cfg)
{

    this->number_of_column = frame_text_config.number_of_column;
    this->number_of_line = frame_text_config.number_of_line;
    this->font = frame_text_config.font;
    this->tab_size = frame_text_config.tab_size;
    this->fg_color = frame_text_config.fg_color;
    this->bg_color = frame_text_config.bg_color;
    this->wrap = frame_text_config.wrap;
    this->auto_next_char = frame_text_config.auto_next_char;
    // this->frame_text_config = text_cfg;

    create_text_buffer();
}

TextualFrameBuffer::TextualFrameBuffer(GraphicDisplayDevice *device,
                                       size_t frame_width,
                                       size_t frame_height,
                                       struct_TextFramebufferConfig text_cfg)
    : GraphicFramebuffer(device, frame_width, frame_height, text_cfg)
{
    this->frame_text_config = text_cfg;
    this->number_of_column = this->pixel_memory.frame_width / this->frame_text_config.font[FONT_WIDTH_INDEX];
    this->number_of_line = this->pixel_memory.frame_height / this->frame_text_config.font[FONT_HEIGHT_INDEX];

    create_text_buffer();
}

TextualFrameBuffer::~TextualFrameBuffer()
{
    delete[] this->text_buffer;
}

void TextualFrameBuffer::update_text_buffer(struct_TextFramebufferConfig _frame_text_config)
{
    this->frame_text_config = _frame_text_config;
    update_text_area(this->frame_text_config.font);
}

void TextualFrameBuffer::clear_text_buffer()
{
    memset(this->text_buffer, '\0', this->text_buffer_size);
    current_char_column = 0;
    current_char_line = 0;
}

void TextualFrameBuffer::update_text_area(const unsigned char *font)
{
    this->frame_text_config.font = font;

    // size the text area according to the available room within the frame whidth and height
    this->number_of_line = this->pixel_memory.frame_height / frame_text_config.font[FONT_HEIGHT_INDEX];
    this->number_of_column = this->pixel_memory.frame_width / frame_text_config.font[FONT_WIDTH_INDEX];

    delete[] this->text_buffer;
    create_text_buffer();
}

void TextualFrameBuffer::update_pixel_area(const unsigned char *font)
{
    this->frame_text_config.font = font;
    // size the pixel buffer to the required size due to character area
    this->pixel_memory.frame_height = this->number_of_line * frame_text_config.font[FONT_HEIGHT_INDEX];
    this->pixel_memory.frame_width = this->number_of_column * frame_text_config.font[FONT_WIDTH_INDEX];

    delete[] ((GraphicDisplayDevice *)display_screen)->pixel_memory.pixel_buffer;
    ((GraphicDisplayDevice *)display_screen)->create_pixel_buffer(&this->pixel_memory);
}

void TextualFrameBuffer::print_text()
{
    print_text(this->text_buffer);
}

void TextualFrameBuffer::print_text(const char *c_str)
{
    uint16_t n = 0;
    while (c_str[n] != '\0')
    {
        print_char(c_str[n]);
        n++;
    }
}

void TextualFrameBuffer::print_char(char c)
{
    switch (c)
    {
    case VERTICAL_TAB:
        break;
    case LINE_FEED:
        next_line();
        current_char_column = 0;
        break;
    case BACKSPACE:
        current_char_column--;
        drawChar(' ', current_char_column, current_char_line);
        break;
    case FORM_FEED:
        ((GraphicDisplayDevice *)display_screen)->clear_pixel_buffer(&this->pixel_memory);
        current_char_column = 0;
        current_char_line = 0;
        break;
    case CARRIAGE_RETURN:
        current_char_column = 0;
        break;
    default:
        if (current_char_column == 0)
            clear_line(); // start a new line
        if (c == HORIZONTAL_TAB)
        {
            for (uint8_t i = 0; i < frame_text_config.tab_size; i++)
            {
                drawChar(' ', current_char_column, current_char_line);
                next_char();
            }
        }
        else
        {
            if (this->frame_text_config.auto_next_char)
            {
                drawChar(c, current_char_column, current_char_line);
                next_char();
            }
            else
            {
                drawChar(' ', current_char_column, current_char_line);
                drawChar(c, current_char_column, current_char_line);
            }
        }
        break;
    }
}

void TextualFrameBuffer::next_line()
{
    current_char_column = 0;
    current_char_line++;
    if (current_char_line >= number_of_line)
        current_char_line = 0;
}

void TextualFrameBuffer::next_char()
{
    current_char_column++;
    if (current_char_column >= number_of_column)
    {
        if (this->frame_text_config.wrap)
        {
            current_char_column = 0;
            next_line();
        }
    }
}
