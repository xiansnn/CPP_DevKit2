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
                             size_t height,
                             FramebufferFormat format)
{
    this->pixel_memory.frame_height= height;
    this->pixel_memory.frame_width = width;
    this->frame_format = format;
}

DisplayDevice::~DisplayDevice()
{
}
