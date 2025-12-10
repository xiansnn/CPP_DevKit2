#include "canvas.h"
#include <string.h>

// #define PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR //usefull to debug canvas construction/destruction

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
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf("+ Canvas\n");
#endif // MACRO

    this->canvas_width_pixel = canvas_width_pixel;
    this->canvas_height_pixel = canvas_height_pixel;
    this->canvas_buffer_size_pixel = canvas_width_pixel * canvas_height_pixel;
}

Canvas::~Canvas()
{
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf("- Canvas \n");
#endif // MACRO
}

void Canvas::clear_canvas_buffer()
{
    memset(canvas_buffer, 0x00, canvas_buffer_size_byte);
}

void CanvasHMSB::create_canvas_buffer()
{
    size_t byte_per_row = (canvas_width_pixel + BYTE_SIZE - 1) / BYTE_SIZE; // 8 horizontal pixels per byte
    canvas_buffer_size_byte = canvas_height_pixel * byte_per_row;
    canvas_buffer = new uint8_t[canvas_buffer_size_byte];
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf(" CanvasHMSB::create canvas_buffer: %d bytes (%d x %d pixels, %d bytes/row)\n",
           canvas_buffer_size_byte, canvas_width_pixel, canvas_height_pixel, byte_per_row);
#endif // MACRO
    clear_canvas_buffer();
}

CanvasHMSB::CanvasHMSB(uint8_t canvas_width_pixel, uint8_t canvas_height_pixel)
    : Canvas(canvas_width_pixel, canvas_height_pixel)
{
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf("+ CanvasHMSB\t");
#endif // MACRO
    canvas_format = CanvasFormat::MONO_HMSB;
    create_canvas_buffer();
}

CanvasHMSB::~CanvasHMSB()
{
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf("- CanvasHMSB\tdelete[] canvas_buffer\n");
#endif // MACRO
    delete[] canvas_buffer;
}
void CanvasHMSB::fill_canvas_with_color(ColorIndex color)
{
    if (color == bg_color)
        memset(canvas_buffer, 0x00, canvas_buffer_size_byte);
    else
        memset(canvas_buffer, 0xFF, canvas_buffer_size_byte);
}

void CanvasHMSB::draw_pixel(const int x, const int y, const ColorIndex color)
{
    if (x >= 0 && x < canvas_width_pixel && y >= 0 && y < canvas_height_pixel) // avoid drawing outside the canvas
    {
        const int bytes_per_row = (canvas_width_pixel + BYTE_SIZE - 1) / BYTE_SIZE; // x pixels, 1bpp, but 1 byte is 8 column
        int byte_index = y * bytes_per_row + x / BYTE_SIZE;
        uint8_t byte = canvas_buffer[byte_index];

        if (color == fg_color)
            byte |= 0b10000000 >> (x & 0b00000111);
        else
            byte &= ~(0b10000000 >> (x & 0b00000111));

        canvas_buffer[byte_index] = byte;
    }
}

void CanvasVLSB::create_canvas_buffer()
{
    size_t nb_of_pages = (canvas_height_pixel + BYTE_SIZE - 1) / BYTE_SIZE; // 8 vertical pixels per byte
    canvas_buffer_size_byte = canvas_width_pixel * nb_of_pages;
    canvas_buffer = new uint8_t[canvas_buffer_size_byte];
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf(" CanvasVLSB::create canvas_buffer: %d bytes (%d x %d pixels, %d pages)\n",
           canvas_buffer_size_byte, canvas_width_pixel, canvas_height_pixel, nb_of_pages);
#endif // MACRO
    clear_canvas_buffer();
}

CanvasVLSB::CanvasVLSB(uint8_t canvas_width_pixel, uint8_t canvas_height_pixel)
    : Canvas(canvas_width_pixel, canvas_height_pixel)
{
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf("+ CanvasVLSB\t");
#endif // MACRO
    canvas_format = CanvasFormat::MONO_VLSB;
    create_canvas_buffer();
}

CanvasVLSB::~CanvasVLSB()
{
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf("- CanvasVLSB\tdelete[] canvas_buffer\n");
#endif // MACRO
    delete[] canvas_buffer;
}

void CanvasVLSB::fill_canvas_with_color(ColorIndex color)
{
    if (color == ColorIndex::BLACK)
        memset(canvas_buffer, 0x00, canvas_buffer_size_byte);
    else
        memset(canvas_buffer, 0xFF, canvas_buffer_size_byte);
}

void CanvasVLSB::draw_pixel(const int x, const int y, const ColorIndex color)
{
    if (x >= 0 && x < canvas_width_pixel && y >= 0 && y < canvas_height_pixel) // avoid drawing outside the canvas
    {
        const int bytes_per_row = canvas_width_pixel; // x pixels, 1bpp, but each row is 8 pixel high, so (x / 8) * 8
        int byte_index = (y / BYTE_SIZE) * bytes_per_row + x;
        uint8_t byte = canvas_buffer[byte_index];

        if (color != ColorIndex::BLACK)
            byte |= 1 << (y % BYTE_SIZE);
        else
            byte &= ~(1 << (y % BYTE_SIZE));

        canvas_buffer[byte_index] = byte;
    }
}

void CanvasRGB::create_canvas_buffer()
{
    canvas_buffer_size_byte = canvas_buffer_size_pixel; // 1 byte per pixel
    canvas_buffer = new uint8_t[canvas_buffer_size_byte];
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf(" CanvasRGB::create canvas_buffer: %d bytes (%d x %d pixels)\n",
           canvas_buffer_size_byte, canvas_width_pixel, canvas_height_pixel);
#endif // MACRO
    clear_canvas_buffer();
}

CanvasRGB::CanvasRGB(uint8_t canvas_width_pixel, uint8_t canvas_height_pixel)
    : Canvas(canvas_width_pixel, canvas_height_pixel)
{
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf("+ CanvasRGB\t");
#endif // MACRO
    canvas_format = CanvasFormat::RGB_COLOR_INDEX_8b;
    create_canvas_buffer();
}

CanvasRGB::~CanvasRGB()
{
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf("- CanvasRGB\tdelete[] canvas_buffer\n");
#endif // MACRO
    delete[] canvas_buffer;
}

void CanvasRGB::fill_canvas_with_color(ColorIndex color)
{
    memset(canvas_buffer, static_cast<uint8_t>(color), canvas_buffer_size_byte);
}

void CanvasRGB::draw_pixel(const int x, const int y, const ColorIndex color)
{
    if (x >= 0 && x < canvas_width_pixel && y >= 0 && y < canvas_height_pixel)
    {
        int byte_index = y * canvas_width_pixel + x;
        canvas_buffer[byte_index] = static_cast<uint8_t>(color);
    }
}

void CanvasTrueRGB::create_canvas_buffer()
{
    canvas_buffer_size_byte = canvas_buffer_size_pixel * 2; // 2 bytes per pixel
    canvas_16buffer = new uint16_t[canvas_buffer_size_pixel];
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf(" CanvasTrueRGB::create canvas_16buffer: %d bytes (%d x %d pixels)\n",
           canvas_buffer_size_byte, canvas_width_pixel, canvas_height_pixel);
#endif // MACRO
    clear_canvas_buffer();
}

CanvasTrueRGB::CanvasTrueRGB(uint8_t canvas_width_pixel, uint8_t canvas_height_pixel)
    : Canvas(canvas_width_pixel, canvas_height_pixel)
{
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf("+ CanvasTrueRGB\t");
#endif // MACRO
    canvas_format = CanvasFormat::RGB565_16b;
    create_canvas_buffer();
}

CanvasTrueRGB::~CanvasTrueRGB()
{
#if defined(PRINT_CANVAS_CONSTRUCTOR_DESTRUCTOR)
    printf("- CanvasTrueRGB\tdelete[] canvas_16buffer\n");
#endif // MACRO
    delete[] canvas_16buffer;
}

void CanvasTrueRGB::clear_canvas_buffer()
{
    memset(canvas_16buffer, 0x00, canvas_buffer_size_byte);
}

void CanvasTrueRGB::fill_canvas_with_color(ColorIndex color)
{
    for (size_t i = 0; i < canvas_buffer_size_pixel; i++)
    {
        canvas_16buffer[i] = color565_palette[color];
    }
}

void CanvasTrueRGB::draw_pixel(const int x, const int y, const ColorIndex color)
{
    if (x >= 0 && x < canvas_width_pixel && y >= 0 && y < canvas_height_pixel)
    {
        int byte_index = y * canvas_width_pixel + x;
        canvas_16buffer[byte_index] = color565_palette[color];
    }
}

/* Code we may need
//==================================================================
static uint8_t reversed[sizeof(font)] = {0};

static uint8_t reverse(uint8_t b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}
static void FillReversedCache()
{
    // calculate and cache a MONO_VLSB version of fhe font, because it's defined MONO_VMSB (upside down)
    for (size_t i = 0; i < sizeof(font); i++)
        reversed[i] = reverse(font[i]);
}
//=============================================================
*/
