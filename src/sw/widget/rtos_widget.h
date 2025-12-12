/**
 * @file rtos_widget.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <vector>
#include <string>

#include "pico/stdlib.h"

#include "sw/display_device/display_device.h"
#include "sw/widget/canvas.h"
#include "sw/widget/widget_primitives.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

class rtos_Model;

/// @brief RTOS wrapper for Widget class
/// \ingroup view
class rtos_Widget
{
protected:
    /// @brief a pointer to the Model actually displayed by the widget
    rtos_Model *actual_rtos_displayed_model{nullptr};

    /// @brief A rtos_widget can be composed by several rtos_widgets.
    std::vector<rtos_Widget *> rtos_widgets;

public:
    /// @brief the display device where the attached to the frame buffer
    rtos_DisplayDevice *display_device{nullptr};
    /// @brief location in x of the widget within the hosting framebuffer
    uint8_t widget_anchor_x;
    /// @brief location in y of the widget within the hosting framebuffer
    uint8_t widget_anchor_y;

    struct_WidgetDataToGateKeeper widget_data_to_gatekeeper;

    /// @brief FreeRTOS task handle associated to the widget
    TaskHandle_t task_handle;

    /// @brief constructor for RTOS widget
    /// @param actual_displayed_model the displayed model of the widget
    /// @param display_device The display device on which the widget is drawn. This device can be "null".
    rtos_Widget(rtos_Model *actual_displayed_model,
                rtos_DisplayDevice *display_device = nullptr);

    /// @brief destructor for RTOS widget
    ~rtos_Widget();
    /// @brief Set the display screen object
    /// @param _new_display_device
    void set_display_device(rtos_DisplayDevice *_new_display_device);
    /// @brief Modify the anchor of the widget on the display screen
    /// @param x anchor x coordinate
    /// @param y anchor y coordinate
    void update_widget_anchor(uint8_t x, uint8_t y);

    /// @brief add sub_widget to the current widget
    /// @param _sub_widget
    void add_widget(rtos_Widget *_sub_widget);

    /// @brief a pure virtual member that is called to effectively draw the widget.
    /// \note USAGE: This member function can be called by the draw_refresh_all_attached_widgets() method of the Model.
    /// Refer to the following diagram.
    /// \image html draw.svg
    virtual void draw() = 0;
    /// @brief A pure virtual method that results in the transfer of the displayed values of the displayed model to the widget.
    virtual void get_value_of_interest() = 0;
};

class rtos_GraphicWidget : public rtos_Widget
{
private:
public:
    GraphicDrawer *drawer;
    rtos_GraphicWidget(rtos_Model *actual_displayed_model,
                       struct_ConfigGraphicWidget graph_cfg,
                       CanvasFormat canvas_format,
                       rtos_DisplayDevice *display_device);
    virtual ~rtos_GraphicWidget();
};

class rtos_TextWidget : public rtos_Widget
{
private:
public:
    TextWriter *writer;
    /// @brief Construct a new Text Widget object
    /// \note USAGE: when the text frame is defined by the number of characters width and height.
    /// @param graphic_display_screen The display device on which the widget is drawn
    /// @param text_cfg the configuration data for the textual frame
    /// @param canvas_format the format of the associated canvas (see CanvasFormat)
    /// @param displayed_object the displayed model of the widget. Default to nullptr
    rtos_TextWidget(rtos_Model *actual_displayed_model,
                    struct_ConfigTextWidget text_cfg,
                    CanvasFormat canvas_format,
                    rtos_DisplayDevice *display_device);

    /// @brief Construct a new Text Widget object
    /// \note USAGE: when the text frame is defined by the frame size width and height in pixel.
    /// @param graphic_display_screen The display device on which the widget is drawn
    /// @param text_cfg the configuration data for the textual frame
    /// @param canvas_format the format of the associated canvas (see CanvasFormat)
    /// @param frame_width the frame size width
    /// @param frame_height the frame size height
    /// @param displayed_object the displayed model of the widget. Default to nullptr
    rtos_TextWidget(rtos_Model *actual_displayed_model,
                    struct_ConfigTextWidget text_cfg,
                    CanvasFormat canvas_format,
                    size_t frame_width, size_t frame_height,
                    rtos_DisplayDevice *display_device);
    virtual ~rtos_TextWidget();
};

class rtos_PrintWidget : public rtos_Widget
{
protected:
    /// @brief the size, in number of character of a line
    size_t number_of_column;
    /// @brief the number of line
    size_t number_of_line;
    /// @brief  the number of characters
    size_t text_buffer_size;

public:
    /// @brief the effective character buffer
    char *text_buffer = nullptr;
    rtos_PrintWidget(rtos_Model *actual_displayed_model,
                     rtos_TerminalConsole *terminal_console);
    virtual ~rtos_PrintWidget();
};
