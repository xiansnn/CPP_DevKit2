/**
 * @file widget_focus_indicator.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once
#include "widget_square_led.h"
#include "ui_core.h"


/**
 * @brief WidgetFocusIndicator : this is a special led widget used to show the status of the model.
 * 
 * It is supposed to work with another widget that shows the expected visualisation of the displayed model.
 */
class WidgetFocusIndicator : public WidgetSquareLed
{
private:

public:
    WidgetFocusIndicator(UIModelObject *actual_displayed_model,
                     DisplayDevice *display_screen,
                     size_t width,
                     size_t height,
                     uint8_t widget_anchor_x,
                     uint8_t widget_anchor_y,
                     bool with_border);
    ~WidgetFocusIndicator();
    void draw_refresh();
};