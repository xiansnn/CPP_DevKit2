/**
 * @file widget_bargraph.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "framebuffer.h"
#include "widget.h"
#include "display_device.h"

#include "ui_core.h"

/**
 * @brief the ModelObject used by Bargraph GraphicWidget
 *
 */
class ModelBargraph : public ModelObject  // TODO prevoir ModelObject au lieu de ModelObject car pas besoin UI
{
private:
public:
    /// @brief the vector of values displayed by the bargraph
    std::vector<int> values;
    /// @brief the  number of bar in the bargraph (and the number of values)
    size_t number_of_bar;
    /// @brief the min value of the range of values
    int min_value;
    /// @brief the max value of the range of values
    int max_value;
    /// @brief conctructor of ModelBargraph
    /// @param number_of_bar the  number of bar in the bargraph
    /// @param min_value the min value of the range of values
    /// @param max_value the max value of the range of values
    ModelBargraph(size_t number_of_bar, int min_value, int max_value);
    /**
     * @brief Destroy the Model Bargraph object
     *
     */
    ~ModelBargraph();
    /**
     * @brief the overriding memeber required by ModelObject.
     * There is no need of UIControlEvent here, this member just trigs the changere flag in order to trig the refresh of the widget
     *
     * @param event Default to NONE
     */
    void process_control_event(UIControlEvent event = UIControlEvent::NONE);
};

/**
 * @brief the widget that display an horizontal bargraph
 *
 */
class WidgetHorizontalBargraph : public GraphicWidget
{
private:
    /// @brief the number of pixel that separates each bar (usually 1 pixel)
    uint8_t bar_spacing;
    /// @brief  the computed height of each bar, according to the widget height, the number of bar and the space between bars
    uint8_t bar_height;
    /// @brief the computed bar width. Usually the widget width.
    uint8_t bar_width;

    /// @brief the computed equivalent of max level value of the bar in x-coordinate
    uint8_t px_max;
    /// @brief the computed equivalent of min level value of the bar in x-coordinate
    uint8_t px_min;
    /// @brief the computed proportional coeficient of the value to pixel converter
    float level_coef;
    /// @brief the computed offset of the value to pixel converter
    int level_offset;
    /// @brief the value to pixel x-coordinate converter
    /// @param level the input value
    /// @return the x-coordinate
    uint8_t convert_level_value_to_px(int level);
    /// @brief the function that draw a bar in the widget
    /// @param bin_index the rank of the bar
    void draw_bar(uint8_t bin_index);

protected:
public:
    /// @brief the function that draw the widget
    void draw();
    /**
     * @brief Construct a new GraphicWidget Horizontal Bargraph object
     *
     * @param bargraph_model a pointer to the mactual displayed model
     * @param graphic_display_screen a pointer to the display device on which the widget is drawn
     * @param graph_cfg the configuration data structure of the graphic framebuffer
     * @param widget_anchor_x the horizontal position where the widget start on the device screen
     * @param widget_anchor_y the vertical position where the widget start on the device screen
     * @param widget_with_border the flag that indicates whether the widget has a border or not
     * @param bar_spacing the number of pixel between each bar
     */
    WidgetHorizontalBargraph(ModelBargraph *bargraph_model,
                             GraphicDisplayDevice *graphic_display_screen,
                             struct_ConfigGraphicFramebuffer graph_cfg,
                             uint8_t widget_anchor_x,
                             uint8_t widget_anchor_y,
                             bool widget_with_border,
                             uint8_t bar_spacing = 1);
    /**
     * @brief Destroy the GraphicWidget Horizontal Bargraph object
     *
     */
    ~WidgetHorizontalBargraph();
};

/**
 * @brief the widget that display a vertical bargraph
 *
 */
class WidgetVerticalBargraph : public GraphicWidget
{
private:
    /// @brief the number of pixel that separates each bar (usually 1 pixel)
    uint8_t bar_spacing;
    /// @brief the computed bar height. Usually the widget height.
    uint8_t bar_height;
    /// @brief the computed width of each bar, according to the widget width, the number of bar and the space between bars
    uint8_t bar_width;
    /// @brief the computed equivalent of max level value of the bar in y-coordinate
    uint8_t py_max;
    /// @brief the computed equivalent of min level value of the bar in y-coordinate
    uint8_t py_min;
    /// @brief the computed proportional coeficient of the value to pixel converter
    float level_coef;
    /// @brief the computed offset of the value to pixel converter
    int level_offset;
    /// @brief the value to pixel y-coordinate converter
    /// @param level the input value
    /// @return the y-coordinate
    uint8_t convert_level_value_to_py(int level);
    /// @brief the function that draw a bar in the widget
    /// @param bin_index the rank of the bar
    void draw_bar(uint8_t bin_index);
    
    public:
    /// @brief the function that draw the widget
    void draw();
    
    /**
     * @brief Construct a new GraphicWidget Vertical Bargraph object
     *
     * @param bargraph_model a pointer to the mactual displayed model
     * @param graphic_display_screen a pointer to the display device on which the widget is drawn
     * @param graph_cfg the configuration data structure of the graphic framebuffer
     * @param widget_anchor_x the horizontal position where the widget start on the device screen
     * @param widget_anchor_y the vertical position where the widget start on the device screen
     * @param widget_with_border the flag that indicates whether the widget has a border or not
     * @param bar_spacing the number of pixel between each bar
     */
    WidgetVerticalBargraph(ModelBargraph *bargraph_model,
                           GraphicDisplayDevice *graphic_display_screen,
                           struct_ConfigGraphicFramebuffer graph_cfg,
                           uint8_t widget_anchor_x,
                           uint8_t widget_anchor_y,
                           bool widget_with_border,
                           uint8_t bar_spacing = 1);
    ~WidgetVerticalBargraph();
};
