/**
 * @file display_device.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#if !defined(DISPLAY_DEVICE_H)
#define DISPLAY_DEVICE_H

#include "framebuffer.h"

/**
 * @brief This is the abstract class to handle all generic behavior of physical display devices (e.g. OLED screen SSD1306).
 * It derived from Framebuffer. This allows to draw text and graphics directly into the display framebuffer
 * thanks to Framebuffet class text and graphic primitives indepently from any kind of widget
 *
 */
class DisplayDevice : public Framebuffer//TODO METTRE UN FRAMEBUFFER EN COLLABORATION
{
private:
public:
    /**
     * @brief Construct a new UIDisplayDevice object
     *
     * @param width The width of physical screen, in pixel.
     * @param height The height of physical screen, in pixel.
     * @param format The framebuffer format ... see Framebuffer class FramebufferFormat enumeration
     * @param txt_cfg A structure of data that reflect the framebuffer member related to text primitives
     */
    DisplayDevice(size_t width,
                  size_t height,
                  struct_GraphFramebuffer graph_cfg = {},
                  FramebufferFormat format = FramebufferFormat::MONO_VLSB);

    /**
     * @brief Destroy the UIDisplayDevice object
     *
     */
    virtual ~DisplayDevice();
    /**
     * @brief This is an pure virtual member function that all final derived class must implement.
     * It transfers the framebuffer buffer to the entire display screen buffer.
     *
     */
    virtual void show() = 0;
    /**
     * @brief This is an pure virtual member function that all final derived class must implement.
     * It transfers the framebuffer buffer to the a part of display screen buffer starting at the (anchor_x, anchor_y) coordinates of the screen , expressed in pixel.
     *
     * @param frame a pointer to the frame to be displayed
     * @param anchor_x the x (horizontal)starting position of the frame within the display screen, (in pixel)
     * @param anchor_y the y (vertical) starting position of the frame within the display screen, (in pixel)
     */
    virtual void show(Framebuffer *frame, uint8_t anchor_x, uint8_t anchor_y) = 0;
};

#endif // DISPLAY_DEVICE_H
