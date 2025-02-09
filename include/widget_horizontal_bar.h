/**
 * @file widget_horizontal_bar.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "ui_core.h"
#include "widget.h"

/// @brief a widget that draw an horizontal bar, filled up to the given level value
class WidgetHorizontalBar : public Widget
{
private:
    /// @brief the filled level of the widget
    int level;
    /// @brief the max value of the level
    int max_value;
    /// @brief the min value of the level
    int min_value;
    /// @brief the x-position of the max level value
    uint8_t px_max;
    /// @brief the y-position of the min level value
    uint8_t px_min;
    /// @brief the converted x-position pixel for the level value
    uint8_t px_position;
    /// @brief  the proportional coefficient of the conversion level-> pixel
    float level_coef;
    /// @brief the offset of the conversion level-> pixel
    int level_offset;

    /// @brief the conversion fucntion level -> value
    void convert_level_value_to_px();

    /// @brief the draw function of the widget
    void draw();

protected:
public:   
    /**
     * @brief Construct a new Widget Horizontal Bar object
     * 
     * @param bar_value_model a pointer to the displayed model
     * @param graphic_display_screen a pointer to the display device
     * @param max_value max value of the bar
     * @param min_value min value of the bar
     * @param graph_cfg the configuration data for the graphic framebuffer
     * @param widget_anchor_x the pixel x-location of the widget
     * @param widget_anchor_y the pixel y-location of the widget (remind. must be multiple of 8)
     * @param widget_with_border true if the widget has a border( it's prettier with a border)
     */
     WidgetHorizontalBar(UIModelObject *bar_value_model,
                        GraphicDisplayDevice *graphic_display_screen,
                        int max_value, int min_value,
                        struct_ConfigGraphicFramebuffer graph_cfg,
                        uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                        bool widget_with_border = true);

    ~WidgetHorizontalBar();
    /// @brief set the level to value
    /// @param value
    void set_level(int value);

    virtual void draw_refresh();
};
