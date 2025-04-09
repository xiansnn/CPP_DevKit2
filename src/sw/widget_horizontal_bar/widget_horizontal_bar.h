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

#include "sw/ui_core/ui_core.h"
#include "sw/widget/widget.h"


/// @brief a widget that draw an horizontal bar, filled up to the given level value
/// \ingroup view
class WidgetHorizontalBar : public GraphicWidget
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

protected:
public:
    /// @brief the draw function of the widget
    void draw();

    /// @brief Construct a new GraphicWidget Horizontal Bar object
    /// @param bar_value_model a pointer to the displayed model
    /// @param graphic_display_screen a pointer to the display device
    /// @param max_value max value of the bar
    /// @param min_value min value of the bar
    /// @param graph_cfg the configuration data for the graphic framebuffer
    WidgetHorizontalBar(Model *bar_value_model,
                        GraphicDisplayDevice *graphic_display_screen,
                        int max_value, int min_value,
                        struct_ConfigGraphicWidget graph_cfg,
                        CanvasFormat format);

    ~WidgetHorizontalBar();
    /// @brief set the level to value
    /// @param value
    void set_level(int value);
};
