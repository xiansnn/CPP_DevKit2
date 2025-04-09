#include "canvas.h"
#include <string.h>

#define RED_MASK 0xF8
#define GREEN_MASK 0xFC
#define BLUE_MASK 0xF8
#define RGB_TO_565(R, G, B) (((R & RED_MASK) << 8) | ((G & GREEN_MASK) << 3) | (B & BLUE_MASK) >> 3)

std::map<ColorIndex, uint16_t> color565_palette{
    {ColorIndex::BLACK, RGB_TO_565(0, 0, 0)},
    {ColorIndex::WHITE, RGB_TO_565(255, 255, 255)},
    {ColorIndex::RED, RGB_TO_565(255, 0, 0)},
    {ColorIndex::LIME, RGB_TO_565(0, 255, 0)},
    {ColorIndex::BLUE, RGB_TO_565(0, 0, 255)},
    {ColorIndex::YELLOW, RGB_TO_565(255, 255, 0)},
    {ColorIndex::CYAN, RGB_TO_565(0, 255, 255)},
    {ColorIndex::MAGENTA, RGB_TO_565(255, 0, 255)},
    {ColorIndex::SILVER, RGB_TO_565(192, 192, 192)},
    {ColorIndex::GRAY, RGB_TO_565(128, 128, 128)},
    {ColorIndex::BURGUNDY, RGB_TO_565(128, 0, 0)},
    {ColorIndex::MAROON, RGB_TO_565(165, 42, 42)},
    {ColorIndex::OLIVE, RGB_TO_565(128, 128, 0)},
    {ColorIndex::GREEN, RGB_TO_565(0, 128, 0)},
    {ColorIndex::PURPLE, RGB_TO_565(128, 0, 128)},
    {ColorIndex::TEAL, RGB_TO_565(0, 128, 128)},
    {ColorIndex::NAVY, RGB_TO_565(0, 0, 128)},
    {ColorIndex::ORANGE, RGB_TO_565(255, 165, 0)},
    {ColorIndex::GOLD, RGB_TO_565(255, 215, 0)},
    {ColorIndex::FOREST, RGB_TO_565(34, 139, 34)}};

Canvas::Canvas(uint8_t canvas_width_pixel, uint8_t canvas_height_pixel)
{
    this->pixel_frame_width = canvas_width_pixel;
    this->pixel_frame_height = canvas_height_pixel;
}

Canvas::~Canvas()
{
    delete pixel_frame_buffer;
}

void Canvas::clear_canvas_buffer()
{
    memset(pixel_frame_buffer, 0x00, pixel_frame_buffer_size);
}

struct_PixelFrame Canvas::get_pixel_frame()
{
    struct_PixelFrame pixel_frame ={
        .pixel_frame_width = this->pixel_frame_width,
        .pixel_frame_height = this->pixel_frame_height,
        .pixel_frame_buffer_size = this->pixel_frame_buffer_size,
        .pixel_frame_buffer = this->pixel_frame_buffer
    };
    return pixel_frame;
}

void CanvasVLSB::create_canvas_buffer()
{
    size_t nb_of_pages = (pixel_frame_height + BYTE_SIZE - 1) / BYTE_SIZE;
    pixel_frame_buffer_size = pixel_frame_width * nb_of_pages;
    pixel_frame_buffer = new uint8_t[pixel_frame_buffer_size];
    clear_canvas_buffer();
}

CanvasVLSB::CanvasVLSB(uint8_t canvas_width_pixel, uint8_t canvas_height_pixel)
    : Canvas(canvas_width_pixel, canvas_height_pixel)
{
    create_canvas_buffer();
}

CanvasVLSB::~CanvasVLSB()
{
}

void CanvasVLSB::pixel(const int x, const int y, const ColorIndex color)
{
    if (x >= 0 && x < pixel_frame_width && y >= 0 && y < pixel_frame_height) // avoid drawing outside the canvas
    {
        const int bytes_per_row = pixel_frame_width; // x pixels, 1bpp, but each row is 8 pixel high, so (x / 8) * 8
        int byte_index = (y / BYTE_SIZE) * bytes_per_row + x;
        uint8_t byte = pixel_frame_buffer[byte_index];

        if (color == ColorIndex::WHITE)
            byte |= 1 << (y % BYTE_SIZE);
        else
            byte &= ~(1 << (y % BYTE_SIZE));

        pixel_frame_buffer[byte_index] = byte;
    }
}

void CanvasVLSB::draw_char_into_pixel(const struct_ConfigTextWidget text_config, const char character, const uint8_t anchor_x, const uint8_t anchor_y)
{
    if (!text_config.font || character < 32)
        return;

    uint8_t font_width = text_config.font[FONT_WIDTH_INDEX];
    uint8_t font_height = text_config.font[FONT_HEIGHT_INDEX];

    uint16_t seek = (character - 32) * (font_width * font_height) / 8 + 2;

    uint8_t b_seek = 0;

    for (uint8_t x = 0; x < font_width; x++)
    {
        for (uint8_t y = 0; y < font_height; y++)
        {
            if (text_config.font[seek] >> b_seek & 0b00000001)
                pixel(x + anchor_x, y + anchor_y, text_config.fg_color);
            else
                pixel(x + anchor_x, y + anchor_y, text_config.bg_color);

            b_seek++;
            if (b_seek == 8)
            {
                b_seek = 0;
                seek++;
            }
        }
    }
}

void CanvasRGB::create_canvas_buffer()
{
}

CanvasRGB::CanvasRGB(uint8_t canvas_width_pixel, uint8_t canvas_height_pixel)
    : Canvas(canvas_width_pixel, canvas_height_pixel)
{
    create_canvas_buffer();
}

CanvasRGB::~CanvasRGB()
{
}

void CanvasRGB::pixel(const int x, const int y, const ColorIndex color)
{
}

void CanvasRGB::draw_char_into_pixel(const struct_ConfigTextWidget text_config, const char character, const uint8_t anchor_x, const uint8_t anchor_y)
{
}
