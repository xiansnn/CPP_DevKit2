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
     * @brief Construct a new Widget Square Led object
     * 
     * @param display_screen The display device on which the widget is drawn.
     * @param frame_width The width of the widget including the border
     * @param frame_height the height of the widget including the border
     * @param widget_anchor_x the horizontal position where the widget start on the device screen
     * @param widget_anchor_y the vertical position where the widget start on the device screen
     * @param widget_with_border The flag that indicates whether the widget has a border or not
     * @param fg_color the foreground color
     * @param bg_color the background color
     */
    WidgetSquareLed(GraphicDisplayDevice *display_screen,
                size_t frame_width,
                size_t frame_height,
                uint8_t widget_anchor_x,
                uint8_t widget_anchor_y,
                bool widget_with_border = true,
                PixelColor fg_color = PixelColor::WHITE,
                PixelColor bg_color = PixelColor::BLACK);
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

    virtual void draw_refresh();

};
