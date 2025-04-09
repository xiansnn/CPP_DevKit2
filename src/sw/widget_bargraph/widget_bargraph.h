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

#include "sw/display_device/display_device.h"
#include "sw/ui_core/ui_core.h"
#include "sw//widget/widget.h"

/**
 * @brief the Model used by Bargraph GraphicWidget
 * \ingroup model
 */
class ModelBargraph : public Model
{
private:
protected:
    /// @brief the vector of values displayed by the bargraph
    std::vector<int> values;

public:
    /// @brief get internal values of the bargraph model
    /// @return internal values
    std::vector<int> get_values();

    /// @brief update internal values of the bargraph and set_change_flag
    /// @param values
    void update_values(std::vector<int> values);
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
};

/**
 * @brief the widget that display an horizontal bargraph
 * \ingroup view
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
    /// @brief the set of values for the bargraph
    std::vector<int> values;
    /// @brief number of values in the bargraph
    uint8_t number_of_bar;

public:
    /// @brief the function that draw the widget
    void draw();

    virtual void get_value_of_interest();

    /// @brief Construct a new GraphicWidget Horizontal Bargraph object
    /// @param bargraph_model a pointer to the mactual displayed model
    /// @param graphic_display_screen a pointer to the display device on which the widget is drawn
    /// @param graph_cfg the configuration data structure of the graphic framebuffer
    /// @param bar_spacing the number of pixel between each bar. Default to 1.
    WidgetHorizontalBargraph(ModelBargraph *bargraph_model,
                             GraphicDisplayDevice *graphic_display_screen,
                             struct_ConfigGraphicWidget graph_cfg,
                             CanvasFormat format,
                             uint8_t bar_spacing = 1);
    /**
     * @brief Destroy the GraphicWidget Horizontal Bargraph object
     *
     */
    ~WidgetHorizontalBargraph();
};

/**
 * @brief the widget that display a vertical bargraph
 * \ingroup view
 */
class WidgetVerticalBargraph : public GraphicWidget
{
private:
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

protected:
    /// @brief the set of values for the bargraph
    std::vector<int> values;
    /// @brief number of values in the bargraph
    uint8_t number_of_bar;

public:
    /// @brief the number of pixel that separates each bar (usually 1 pixel)
    uint8_t bar_spacing;
    /// @brief the computed bar height. Usually the widget height.
    uint8_t bar_height;
    /// @brief the computed width of each bar, according to the widget width, the number of bar and the space between bars
    uint8_t bar_width;
    /// @brief the function that draw the widget
    void draw();

    virtual void get_value_of_interest();

    /// @brief Construct a new GraphicWidget Vertical Bargraph object
    /// @param bargraph_model a pointer to the mactual displayed model
    /// @param graphic_display_screen a pointer to the display device on which the widget is drawn
    /// @param graph_cfg the configuration data structure of the graphic framebuffer
    /// @param bar_spacing the number of pixel between each bar. Default to 1.
    WidgetVerticalBargraph(ModelBargraph *bargraph_model,
                           GraphicDisplayDevice *graphic_display_screen,
                           struct_ConfigGraphicWidget graph_cfg,
                           CanvasFormat format,
                           uint8_t bar_spacing = 1);
    ~WidgetVerticalBargraph();
};
