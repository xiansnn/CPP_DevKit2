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

/// @brief enumearion of the stauts of a blinking LED
enum class LEDStatus
{
    /// @brief LED is blinking
    LED_IS_BLINKING,
    /// @brief LED is switched ON
    LED_IS_ON,
    /// @brief LED is switched OFF
    LED_IS_OFF
};

/**
 * @brief A widget that show a square (it can be a rectangle) on the display.
 *
 */
class WidgetSquareLed : public GraphicWidget
{
private:
protected:
    /**
     * @brief the status of the led, on or off
     */
    bool led_is_on;

public:
    /**
     * @brief Construct a new GraphicWidget Square Led object
     *
     * @param actual_displayed_model the actual displayed model
     * @param graphic_display_screen The display device on which the widget is drawn.
     * @param graph_cfg the configuration data structure of the graphic framebuffer
     */
    WidgetSquareLed(Model *actual_displayed_model,
                    GraphicDisplayDevice *graphic_display_screen,
                    struct_ConfigGraphicWidget graph_cfg);
    ~WidgetSquareLed();

    virtual void draw();
};
/**
 * @brief A widget that show a square (it can be a rectangle) on the display.
 *
 */
class WidgetBlinkingSquareLed : public WidgetSquareLed, public Blinker
{
private:
protected:
    /// @brief the operating status of the LED (blinking, ON, OFF)
    LEDStatus led_status;


public:
    /**
     * @brief Construct a new GraphicWidget Square Led object
     *
     * @param actual_displayed_model the actual displayed model
     * @param graphic_display_screen The display device on which the widget is drawn.
     * @param graph_cfg the configuration data structure of the graphic framebuffer
     */
    WidgetBlinkingSquareLed(Model *actual_displayed_model,
                            GraphicDisplayDevice *graphic_display_screen,
                            struct_ConfigGraphicWidget graph_cfg);
    ~WidgetBlinkingSquareLed();

    virtual void draw();
};
