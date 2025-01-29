#include "display_device.h"

DisplayDevice::DisplayDevice(size_t width,
                             size_t height, 
                             struct_GraphFramebuffer graph_cfg,
                             FramebufferFormat format)
    : Framebuffer(width, height, graph_cfg, format)
{
}

DisplayDevice::~DisplayDevice()
{
}
