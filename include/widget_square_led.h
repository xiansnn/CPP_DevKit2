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
class WidgetSquareLed : public GraphicWidget
{
private:
protected:
    /// @brief the member that draw the square led according to the value of led_is_on.
    void draw_led();
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
     * @brief Construct a new GraphicWidget Square Led object
     *
     * @param actual_displayed_model the actual displayed model
     * @param graphic_display_screen The display device on which the widget is drawn.
     * @param graph_cfg the configuration data structure of the graphic framebuffer
     * @param widget_anchor_x the horizontal position where the widget start on the device screen
     * @param widget_anchor_y the vertical position where the widget start on the device screen
     * @param widget_with_border The flag that indicates whether the widget has a border or not
     */
    WidgetSquareLed(Model *actual_displayed_model,
                    GraphicDisplayDevice *graphic_display_screen,
                    struct_ConfigGraphicFramebuffer graph_cfg,
                    uint8_t widget_anchor_x,
                    uint8_t widget_anchor_y,
                    bool widget_with_border = true);
    ~WidgetSquareLed();

    /**
     * @brief draw the square led ON or OFF on blinking phase change
     *
     */
    void blink_refresh();

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
