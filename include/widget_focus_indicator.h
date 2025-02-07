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
    /**
     * @brief Construct a new Widget Focus Indicator object
     * 
     * @param actual_displayed_model  a pointer to the displayed model
     * @param display_screen  a pointer to the dispaly device
     * @param width the width of the widget
     * @param height the height of the widget
     * @param widget_anchor_x the x-location where is the widget on the display device
     * @param widget_anchor_y the y-location where is the widget ont hte display device
     * @param with_border true, if the widget has a border (usually, it's prettier not to have)
     */
    WidgetFocusIndicator(UIModelObject *actual_displayed_model,
                     GraphicDisplayDevice *display_screen,
                     struct_ConfigGraphicFramebuffer graph_cfg,
                     uint8_t widget_anchor_x,
                     uint8_t widget_anchor_y,
                     bool with_border);
    ~WidgetFocusIndicator();
    void draw_refresh();
};