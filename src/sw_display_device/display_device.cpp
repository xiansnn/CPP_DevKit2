/**
 * @file display_device.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-02-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "display_device.h"

GraphicDisplayDevice::GraphicDisplayDevice(size_t screen_width,
                                           size_t screen_height)
    : DisplayDevice(screen_width,screen_height)
{
    this->pixel_memory.frame_height = screen_height;
    this->pixel_memory.frame_width = screen_width;
}

GraphicDisplayDevice::~GraphicDisplayDevice()
{
}

TextDisplayDevice::TextDisplayDevice(size_t number_of_char_width, size_t number_of_char_height)
    : GraphicDisplayDevice(number_of_char_width, number_of_char_height)
{
}

TextDisplayDevice::~TextDisplayDevice()
{
}

DisplayDevice::DisplayDevice(size_t screen_width, size_t screen_height)
{
    this->screen_height = screen_height;
    this->screen_width = screen_width;
}

DisplayDevice::~DisplayDevice()
{
}

PrinterDevice::PrinterDevice(size_t number_of_char_width, size_t number_of_char_height)
    : DisplayDevice(number_of_char_width,number_of_char_height)
{
    this->text_memory.number_of_line = number_of_char_height;
    this->text_memory.number_of_column = number_of_char_width;
    this->text_memory.text_buffer_size = number_of_char_width * number_of_char_height + 1; // +1 because this is a C-style char[] ended with \x0

    this->text_memory.text_buffer = new char[text_memory.text_buffer_size];
}

PrinterDevice::~PrinterDevice()
{
    delete[] this->text_memory.text_buffer;
}
