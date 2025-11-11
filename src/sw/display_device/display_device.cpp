
#include "display_device.h"

GraphicDisplayDevice::GraphicDisplayDevice(size_t screen_width,
                                           size_t screen_height)
{
    this->TFT_panel_height_in_pixel = screen_height;
    this->TFT_panel_width_in_pixel = screen_width;
}

GraphicDisplayDevice::~GraphicDisplayDevice()
{
}

PrinterDevice::PrinterDevice(size_t number_of_char_width, size_t number_of_char_height)
{
    this->number_of_line = number_of_char_height;
    this->number_of_column = number_of_char_width;
    this->text_buffer_size = number_of_char_width * number_of_char_height + 1; // +1 because this is a C-style char[] ended with \x0

    this->text_buffer = new char[text_buffer_size];
}

PrinterDevice::~PrinterDevice()
{
    delete[] this->text_buffer;
}

void PrinterDevice::show()
{
    stdio_printf(this->text_buffer);
}

DisplayDevice::DisplayDevice()
{
}

DisplayDevice::~DisplayDevice()
{
}

rtos_PrinterDevice::rtos_PrinterDevice(size_t number_of_char_width, size_t number_of_char_hight)
    : PrinterDevice(number_of_char_width, number_of_char_hight)
{
}

rtos_PrinterDevice::~rtos_PrinterDevice()
{
}

void rtos_PrinterDevice::show_from_display_queue(char* text_to_print)
{
    stdio_printf(text_to_print);
}

