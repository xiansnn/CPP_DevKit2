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
#include "sw/ui_core/ui_control_event.h"
#include "sw/widget/widget_primitives.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <set>

class rtos_BlinkingWidget;

/// @brief Class that manages blinking widgets
/// \ingroup view
class rtos_Blinker
{
private:
    /// @brief The period of the blinking, in milliseconds
    std::set<rtos_BlinkingWidget *> blinking_widgets;

public:
    /// @brief The period of the blinking, in milliseconds
    uint32_t blink_period_ms;

    /// @brief The current phase of the blinking
    bool current_blink_phase{false};

    /// @brief  Add a blinking widget to the blinker
    /// @param widget   the blinking widget to add
    void add_blinking_widget(rtos_BlinkingWidget *widget);

    /// @brief  Remove a blinking widget from the blinker
    /// @param widget   the blinking widget to remove
    void remove_blinking_widget(rtos_BlinkingWidget *widget);

    /// @brief  Constructor of the RTOS blinker
    /// @param blink_period_ms  the period of the blinking, in milliseconds
    rtos_Blinker(uint32_t blink_period_ms);

    /// @brief Destructor of the RTOS blinker
    ~rtos_Blinker();

    /// @brief  Refresh the blinking state of all blinking widgets
    void refresh_blinking();
};

/// @brief Class that represents a blinking widget
/// \ingroup view
class rtos_BlinkingWidget
{
private:
protected:
    /// @brief the blinker associated to the blinking widget
    rtos_Blinker *blinker;
    /// @brief the blinking state of the widget
    bool is_blinking{false};
    /// @brief backup of the canvas colors
    ColorIndex fg_color_backup;
    /// @brief backup of the canvas background colors
    ColorIndex bg_color_backup;

public:
    /// @brief Constructor of the RTOS blinking widget
    rtos_BlinkingWidget();

    /// @brief Destructor of the RTOS blinking widget
    ~rtos_BlinkingWidget();

    /// @brief  Start the blinking of the widget
    void start_blinking();

    /// @brief  Stop the blinking of the widget
    void stop_blinking();

    /// @brief  Setup the blinker for the blinking widget
    /// @param blinker   the blinker to setup
    void setup_blinking(rtos_Blinker *blinker);

    /// @brief  Convert the status to blinking behavior
    /// @param status
    virtual void convert_status_to_blinking_behavior(ControlledObjectStatus status);

    /// @brief  Save the current canvas colors
    virtual void save_canvas_color() = 0;

    /// @brief  Restore the saved canvas colors
    virtual void restore_canvas_color() = 0;

    /// @brief  Blink the widget
    virtual void blink() = 0;

    /// @brief  Show the focus of the widget
    virtual void show_focus() = 0;
};

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

    /// @brief data structure used to queue widget data to send to the display task
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

    /// @brief  Set the actual displayed model object
    /// @param new_displayed_model  the new displayed model
    void set_actual_displayed_model(rtos_Model *new_displayed_model);

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

//// @brief RTOS wrapper for GraphicWidget class
/// \ingroup view
class rtos_GraphicWidget : public rtos_Widget
{
private:
public:
    /// @brief the graphic drawer associated to the widget
    GraphicDrawer *drawer;
    /// @brief  Constructor of the RTOS graphic widget
    /// @param actual_displayed_model   the displayed model of the widget
    /// @param graph_cfg    the configuration data for the graphical frame
    /// @param canvas_format    the format of the associated canvas (see CanvasFormat)
    /// @param display_device   the display device on which the widget is drawn
    rtos_GraphicWidget(rtos_Model *actual_displayed_model,
                       struct_ConfigGraphicWidget graph_cfg,
                       CanvasFormat canvas_format,
                       rtos_DisplayDevice *display_device);
    /// @brief Destructor of the RTOS graphic widget
    virtual ~rtos_GraphicWidget();
};
/// @brief RTOS wrapper for TextWidget class
/// \ingroup view
class rtos_TextWidget : public rtos_Widget
{
private:
public:
    /// @brief the text writer associated to the widget
    TextWriter *writer;
    /// @brief Construct a new Text Widget object
    /// \note USAGE: when the text frame is defined by the number of characters width and height.
    /// @param actual_displayed_model   the displayed model of the widget
    /// @param text_cfg the configuration data for the textual frame
    /// @param canvas_format the format of the associated canvas (see CanvasFormat)
    /// @param display_device   the display device on which the widget is drawn
    rtos_TextWidget(rtos_Model *actual_displayed_model,
                    struct_ConfigTextWidget text_cfg,
                    CanvasFormat canvas_format,
                    rtos_DisplayDevice *display_device);

    /// @brief Construct a new Text Widget object
    /// \note USAGE: when the text frame is defined by the frame size width and height in pixel.
    /// @param actual_displayed_model   the displayed model of the widget
    /// @param text_cfg the configuration data for the textual frame
    /// @param canvas_format the format of the associated canvas (see CanvasFormat)
    /// @param frame_width the frame size width
    /// @param frame_height the frame size height
    /// @param display_device   the display device on which the widget is drawn
    rtos_TextWidget(rtos_Model *actual_displayed_model,
                    struct_ConfigTextWidget text_cfg,
                    CanvasFormat canvas_format,
                    size_t frame_width, size_t frame_height,
                    rtos_DisplayDevice *display_device);
    virtual ~rtos_TextWidget();
};
/// @brief RTOS wrapper for PrintWidget class
/// \ingroup view
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

    /// @brief
    /// @param actual_displayed_model   the displayed model of the widget
    /// @param terminal_console     the terminal console associated to the print widget
    rtos_PrintWidget(rtos_Model *actual_displayed_model,
                     rtos_TerminalConsole *terminal_console);
    virtual ~rtos_PrintWidget();
};
