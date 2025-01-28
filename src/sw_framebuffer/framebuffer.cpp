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

#define BYTE_SIZE 8

Framebuffer::Framebuffer(size_t _frame_width,
                         size_t _frame_height,
                         FramebufferFormat _framebuffer_format)
{
    this->frame_format = _framebuffer_format;
    assert(this->frame_format == FramebufferFormat::MONO_VLSB); // TODO works only for MONO_VLSB devices

    this->frame_height = _frame_height;
    this->frame_width = _frame_width; // MONO_VLSB => 1 Byte = 1 column of 8 pixel

    create_pixel_buffer();
}

Framebuffer::Framebuffer(uint8_t number_of_column, // utilisé pour heritage textualframebuffer
                         uint8_t number_of_line,
                         struct_FramebufferText text_cnf,
                         FramebufferFormat framebuffer_format)
{
    this->frame_format = framebuffer_format;
    assert(this->frame_format == FramebufferFormat::MONO_VLSB); // TODO works only for MONO_VLSB devices

    this->frame_width = number_of_column * text_cnf.font[FONT_WIDTH_INDEX]; // MONO_VLSB => 1 Byte = 1 column of 8 pixel
    this->frame_height = number_of_line * text_cnf.font[FONT_HEIGHT_INDEX]; // TODO verifier qu'on depasse pas la taille de display

    create_pixel_buffer();
}

Framebuffer::~Framebuffer()
{
    delete[] this->pixel_buffer;
}

FramebufferFormat Framebuffer::get_framebuffer_format()
{
    return this->frame_format;
}

void Framebuffer::fill(FramebufferColor c)
{
    assert(this->frame_format == FramebufferFormat::MONO_VLSB); // TODO works only for MONO_VLSB devices
    if (c == FramebufferColor::BLACK)
        memset(this->pixel_buffer, 0x00, this->pixel_buffer_size);
    else
        memset(this->pixel_buffer, 0xFF, this->pixel_buffer_size);
}

void Framebuffer::clear_pixel_buffer()
{
    fill(FramebufferColor::BLACK);// TODO remplacer black par bg_color
}

void Framebuffer::create_pixel_buffer()
{
    size_t nb_of_pages = frame_height / BYTE_SIZE;
    if (frame_height % BYTE_SIZE != 0)
        nb_of_pages += 1;

    this->pixel_buffer_size = frame_width * nb_of_pages;
    if (this->pixel_buffer != nullptr)//TODO supprimer delete
        delete[] this->pixel_buffer;

    this->pixel_buffer = new uint8_t[this->pixel_buffer_size];
    clear_pixel_buffer();
}

void Framebuffer::pixel(int x, int y, FramebufferColor c)
{
    assert(this->frame_format == FramebufferFormat::MONO_VLSB); // TODO works only for MONO_VLSB devices

    if (x >= 0 && x < this->frame_width && y >= 0 && y < this->frame_height) // avoid drawing outside the framebuffer
    {
        const int BytesPerRow = this->frame_width; // x pixels, 1bpp, but each row is 8 pixel high, so (x / 8) * 8
        int byte_idx = (y / 8) * BytesPerRow + x;
        uint8_t byte = this->pixel_buffer[byte_idx];

        if (c == FramebufferColor::WHITE)
            byte |= 1 << (y % 8);
        else
            byte &= ~(1 << (y % 8));

        this->pixel_buffer[byte_idx] = byte;
    }
}

void Framebuffer::hline(uint8_t x, uint8_t y, size_t w, FramebufferColor c)
{
    for (size_t i = 0; i < w; i++)
        this->pixel(x + i, y, c);
}

void Framebuffer::vline(uint8_t x, uint8_t y, size_t h, FramebufferColor c)
{
    for (size_t i = 0; i < h; i++)
        this->pixel(x, y + i, c);
}

void Framebuffer::line(int x0, int y0, int x1, int y1, FramebufferColor c)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true)
    {
        this->pixel(x0, y0, c);
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

void Framebuffer::rect(uint8_t start_x, uint8_t start_y, size_t w, size_t h, bool fill, FramebufferColor c)
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
                this->pixel(start_x + i_x, start_y + i_y, c);
}

void Framebuffer::ellipse(uint8_t x_center, uint8_t y_center, uint8_t x_radius, uint8_t y_radius, bool fill, uint8_t quadrant, FramebufferColor c)
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
            pixel(x_center + x, y_center + y, c);
            pixel(x_center - x, y_center + y, c);
            pixel(x_center + x, y_center - y, c);
            pixel(x_center - x, y_center - y, c);
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

// void Framebuffer::byteOR(int byte_idx, uint8_t byte)
// {
//     // return if index outside 0 - pixel_buffer length - 1
//     if (byte_idx > (this->pixel_buffer_size - 1))
//         return;
//     this->pixel_buffer[byte_idx] |= byte;
// }
// void Framebuffer::byteAND(int byte_idx, uint8_t byte)
// {
//     // return if index outside 0 - pixel_buffer length - 1
//     if (byte_idx > (this->pixel_buffer_size - 1))
//         return;
//     this->pixel_buffer[byte_idx] &= byte;
// }
// void Framebuffer::byteXOR(int byte_idx, uint8_t byte)
// {
//     // return if index outside 0 - pixel_buffer length - 1
//     if (byte_idx > (this->pixel_buffer_size - 1))
//         return;
//     this->pixel_buffer[byte_idx] ^= byte;
// }

void Framebuffer::circle(int radius, int x_center, int y_center, bool fill, FramebufferColor c)
{
    int x, y, m;
    x = 0;
    y = radius;
    m = 5 - 4 * radius;
    while (x <= y)
    {
        if (!fill)
        {
            pixel(x_center + x, y_center + y, c);
            pixel(x_center + y, y_center + x, c);
            pixel(x_center - x, y_center + y, c);
            pixel(x_center - y, y_center + x, c);
            pixel(x_center + x, y_center - y, c);
            pixel(x_center + y, y_center - x, c);
            pixel(x_center - x, y_center - y, c);
            pixel(x_center - y, y_center - x, c);
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

void TextualFrameBuffer::drawChar(const unsigned char *font, char c, uint8_t anchor_x, uint8_t anchor_y)//TODO integrer dans l’autre draw
{//TODO voir si possible ne garder qu’un seul drawChar, et rename draw_char
    assert(this->frame_format == FramebufferFormat::MONO_VLSB); // TODO works only for MONO_VLSB devices

    if (!font || c < 32)
        return;

    uint8_t font_width = font[FONT_WIDTH_INDEX];
    uint8_t font_height = font[FONT_HEIGHT_INDEX];

    uint16_t seek = (c - 32) * (font_width * font_height) / 8 + 2;

    uint8_t b_seek = 0;

    for (uint8_t x = 0; x < font_width; x++)
    {
        for (uint8_t y = 0; y < font_height; y++)
        {
            if (font[seek] >> b_seek & 0b00000001)
                this->pixel(x + anchor_x, y + anchor_y, this->frame_text_config.fg_color);
            else
                this->pixel(x + anchor_x, y + anchor_y, this->frame_text_config.bg_color);

            b_seek++;
            if (b_seek == 8)
            {
                b_seek = 0;
                seek++;
            }
        }
    }
}

void TextualFrameBuffer::drawChar(char c, uint8_t char_column, uint8_t char_line)
{
    uint8_t anchor_x = char_column * frame_text_config.font[FONT_WIDTH_INDEX];
    uint8_t anchor_y = char_line * frame_text_config.font[FONT_HEIGHT_INDEX];
    drawChar(this->frame_text_config.font, c, anchor_x, anchor_y);
}

void TextualFrameBuffer::clear_line()
{
    for (uint8_t i = 0; i < this->char_width; i++)
    {
        drawChar(' ', i, current_char_line);
    }
}

void TextualFrameBuffer::create_text_buffer()
{
    this->text_buffer_size = char_width * char_height + 1;
    if (text_buffer != nullptr)//TODO supprimer delete
        delete[] text_buffer;
    this->text_buffer = new char[text_buffer_size];
    clear_text_buffer();
}

TextualFrameBuffer::TextualFrameBuffer(uint8_t number_of_column,
                                       uint8_t number_of_line,
                                       struct_FramebufferText text_cnf,
                                       FramebufferFormat framebuffer_format)
    : Framebuffer(number_of_column, number_of_line, text_cnf, framebuffer_format)
{

    this->char_width = number_of_column;
    this->char_height = number_of_line;
    this->frame_text_config = text_cnf;

    create_text_buffer();
}

TextualFrameBuffer::TextualFrameBuffer(size_t frame_width,
                                       size_t frame_height,
                                       FramebufferFormat frame_format,
                                       struct_FramebufferText text_cnf)
    : Framebuffer(frame_width, frame_height, frame_format)
{
    this->frame_text_config = text_cnf;
    this->char_width = this->frame_width / this->frame_text_config.font[FONT_WIDTH_INDEX];
    this->char_height = this->frame_height / this->frame_text_config.font[FONT_HEIGHT_INDEX];

    create_text_buffer();
}

TextualFrameBuffer::~TextualFrameBuffer()
{
    delete[] this->text_buffer;
}

void TextualFrameBuffer::init_text_buffer(struct_FramebufferText _frame_text_config)
{//TODO renommer update_text_buffer
    this->frame_text_config = _frame_text_config;
    set_font(this->frame_text_config.font);
}

void TextualFrameBuffer::clear_text_buffer()
{
    memset(this->text_buffer, '\0', this->text_buffer_size);
    current_char_column = 0;
    current_char_line = 0;
}

void TextualFrameBuffer::set_font(const unsigned char *font)
{//TODO renommer update_font
    assert(this->frame_format == FramebufferFormat::MONO_VLSB); // TODO works only for SSD1306
    this->frame_text_config.font = font;
    // size the pixel buffer to the required size due to character area
    this->frame_height = this->char_height * frame_text_config.font[FONT_HEIGHT_INDEX];
    this->frame_width = this->char_width * frame_text_config.font[FONT_WIDTH_INDEX];
//TODO ajouter delete pixel et text buffer
    create_pixel_buffer();
    create_text_buffer();

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
    case FORM_FEED: //TODO TO CHECK
        clear_pixel_buffer();
        break;
    case CARRIAGE_RETURN: //TODO TO CHECK
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
    if (current_char_line >= char_height)
        current_char_line = 0;
}

void TextualFrameBuffer::next_char()
{
    current_char_column++;
    if (current_char_column >= char_width)
    {
        if (this->frame_text_config.wrap)
        {
            current_char_column = 0;
            next_line();
        }
    }
}
