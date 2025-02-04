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

DisplayDevice::DisplayDevice(size_t width,
                             size_t height)
{
    this->pixel_memory.frame_height= height;
    this->pixel_memory.frame_width = width;

}

DisplayDevice::~DisplayDevice()
{
}

inline TextDisplayDevice::TextDisplayDevice(size_t number_of_char_width, size_t number_of_char_height)
{
    this->number_of_char_height = number_of_char_height;
    this->number_of_char_width = number_of_char_width;
    this->text_buffer_size = number_of_char_width * number_of_char_height + 1;

    this->text_buffer = new char[text_buffer_size];
}

TextDisplayDevice::~TextDisplayDevice()
{
}

