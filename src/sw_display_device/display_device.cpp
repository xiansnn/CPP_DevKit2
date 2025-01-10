#include "display_device.h"

DisplayDevice::DisplayDevice(size_t width, size_t height, FramebufferFormat format, struct_FramebufferText txt_cnf)
    : Framebuffer(width, height, format)
{
}

DisplayDevice::~DisplayDevice()
{
}
