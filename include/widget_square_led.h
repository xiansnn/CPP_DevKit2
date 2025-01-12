/**
 * @file widget_square_led.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "widget.h"
#include "display_device.h"

#pragma once

/**
 * @brief A widget that show a square (it can be a rectangle) on the display.
 * 
 */
class WidgetSquareLed : public Widget
{
protected:
    /**
     * @brief the status of the led, on or off
     */
    bool led_is_on = true;
    /**
     * @brief the blinking status of the led
     */
    bool led_is_blinking = false;
    

public:
    /**
     * @brief Construct a new w SquareLed object
     * 
     * @param display_screen 
     * @param width 
     * @param height 
     * @param widget_anchor_x 
     * @param widget_anchor_y 
     * @param widget_with_border
     * @param color 
     * @param framebuffer_format 
     */
    WidgetSquareLed(DisplayDevice *display_screen,
                size_t width,
                size_t height,
                uint8_t widget_anchor_x,
                uint8_t widget_anchor_y,
                bool widget_with_border = true,
                FramebufferColor color = FramebufferColor::WHITE,
                FramebufferFormat framebuffer_format = FramebufferFormat::MONO_VLSB);
    ~WidgetSquareLed();

    /**
     * @brief set the led ON
     * 
     */
    void light_on();
    /**
     * @brief set the led OFF
     * 
     */
    void light_off();
    
    /**
     * @brief refresh the square led ON or OFF on blinking phase change
     * 
     */
    void blink_refresh();
    /**
     * @brief set the blinking of the led OFF
     * 
     */
    void blink_off();
    /**
     * @brief set the blinking of the led ON
     * 
     */
    void blink_on();




};
