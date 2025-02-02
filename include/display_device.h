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

#include "pico/stdlib.h"

struct struct_PixelMemory
{
    /// @brief The number of pixel along the width of the frame.
    uint8_t frame_width;

    /// @brief The number of pixel along the height of the frame.
    uint8_t frame_height;

    /// @brief the buffer where graphic are drawn
    uint8_t *pixel_buffer = nullptr;

    /// @brief size of the buffer that contains graphics as map of pixels.
    size_t pixel_buffer_size;
};

/// @brief enum used to characterize the type of display device memory organisation
enum class FramebufferFormat
{
    /// @brief define monochrome display with pixel arranged vertically, LSB first
    MONO_VLSB,
    /// @brief define monochrome display with pixel arranged vertically, LSB first
    MONO_HLSB,
    /// @brief define monochrome display with pixel arranged vertically, MSB first
    MONO_HMSB,
    /// @brief define color display with pixel coded on 16-bit word, red-5bit,green-6bit, blue-5bit
    RGB565
};

/// @brief define the binary value for color (limited here to monochome, can be extended later)
enum class FramebufferColor
{
    /// @brief black coded with binary value 0x0
    BLACK = 0,
    /// @brief white coded with binary value 0x1
    WHITE = 1
};

/**
 * @brief This is the abstract class to handle all generic behavior of physical display devices (e.g. OLED screen SSD1306).
 * It derived from Framebuffer. This allows to draw text and graphics directly into the display framebuffer
 * thanks to Framebuffet class text and graphic primitives indepently from any kind of widget
 *
 */
class DisplayDevice
{
protected:
public:
    FramebufferFormat frame_format;

    struct_PixelMemory pixel_memory;
    /**
     * @brief Construct a new Display Device object
     *
     * @param width The width of physical screen, in pixel.
     * @param height The height of physical screen, in pixel.
     * @param graph_cfg the graphical configuration data structure
     * @param format The framebuffer format ... see Framebuffer class FramebufferFormat enumeration
     */
    DisplayDevice(size_t width,
                  size_t height,
                  FramebufferFormat format = FramebufferFormat::MONO_VLSB);

    /**
     * @brief Destroy the UIDisplayDevice object
     *
     */
    virtual ~DisplayDevice();

    /**
     * @brief Fill the pixel_buffer with "0" (BLACK). Reset also character position to (0,0).
     */
    virtual void clear_pixel_buffer(struct_PixelMemory *pixel_memory) = 0;

    /// @brief a common function where the pixel_buffer is created and deleted if already existing
    virtual void create_pixel_buffer(struct_PixelMemory *pixel_memory) = 0;

    /**
     * @brief This is an pure virtual member function that all final derived class must implement.
     * It transfers the framebuffer buffer to the a part of display screen buffer starting at the (anchor_x, anchor_y) coordinates of the screen , expressed in pixel.
     *
     * @param pixel_memory_structure a pointer to the struct_PixelMemory that contains the pixel_buffer to be displayed
     * @param anchor_x the x (horizontal)starting position of the frame within the display screen, (in pixel)
     * @param anchor_y the y (vertical) starting position of the frame within the display screen, (in pixel)
     */
    virtual void show(struct_PixelMemory *pixel_memory_structure, uint8_t anchor_x, uint8_t anchor_y) = 0;

    /**
     * @brief the graphic primitive to draw a pixel
     * 
     * @param pixel_memory_structure 
     * @param x the x position of the pixel
     * @param y the y position of the pixel
     * @param c the color of the pixel
     */
    virtual void pixel(struct_PixelMemory *pixel_memory_structure, int x, int y, FramebufferColor c = FramebufferColor::WHITE) = 0;
};

#endif // DISPLAY_DEVICE_H
