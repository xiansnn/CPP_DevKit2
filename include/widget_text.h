/**
 * @file widget_text.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "ui_core.h"

class WidgetText : Framebuffer
{
private:
    /// @brief store the value of the previous blinking phase.should be 0 or 1.
    int8_t previous_blinking_phase;

protected:
    /// @brief a pointer to the UIModelObject actually displayed by the widget
    UIModelObject *actual_displayed_model = nullptr;

    /// @brief ask if the blinking phase has changed
    /// \return true if phase has changed
    bool blinking_phase_has_changed();

    /// @brief The period of the blinking, in microseconds
    uint32_t blink_period_us;

    /// @brief location in x of the widget within the hosting framebuffer
    uint8_t widget_anchor_x;

    /// @brief location in y of the widget within the hosting framebuffer
    uint8_t widget_anchor_y;

    /// @brief the display device where the widget is displayed
    DisplayDevice *display_screen{nullptr};

    /// @brief if true, the widget is surrounded by a one-pixel border
    bool widget_with_border{true};

    /// @brief A widget can be composed by several widget.
    std::vector<WidgetText *> widgets;

    /// @brief As a widget can be surrounded by a border, the actual widget width is not the associated framebuffer width.
    size_t widget_width{128};

    /// @brief As a widget can be surrounded by a border, the actual widget height is not the associated framebuffer height.
    size_t widget_height{8};
    /**
     * @brief this is the actual horizontal start of the widget drawing area, taken into account the presence of border.
     *
     * WARNING: when the FramebufferFormat format is MONO_VLSB, works fine only if widget_height is a multiple of 8.
     */
    uint8_t widget_start_x;
    /**
     * @brief this is the actual vertical start of the widget drawing area, taken into account the presence of border.
     *
     * WARNING: when the FramebufferFormat format is MONO_VLSB, works fine only if widget_start_y is a multiple of 8
     */
    uint8_t widget_start_y;

    /// @brief this is the border size of the widget
    uint8_t widget_border_width;

    /// @brief draw a rectangle around the widget.
    /// IMPORTANT NOTICE: as the border is a rectangle with fill=false, the border width can only be 1 pixel.
    /// @param c the color of the border
    void draw_border(FramebufferColor c = FramebufferColor::WHITE);

public:
    /**
     * @brief Set the display screen object
     *
     * @param _new_display_device
     */
    void set_display_screen(DisplayDevice *_new_display_device);

    /**
     * @brief Set the blink period in microseconds
     *
     * @param blink_period default to 1 second
     */
    void set_blink_us(uint32_t blink_period = 1000000);

 
    WidgetText(UIModelObject *text_model,
               DisplayDevice *display_screen,
               struct_FramebufferText framebuffer_txt_cnf,
               uint8_t number_of_column,
               uint8_t number_of_line,
               uint8_t widget_anchor_x,
               uint8_t widget_anchor_y,
               bool widget_with_border,
               uint8_t widget_border_width = 1,
               FramebufferFormat framebuffer_format = FramebufferFormat::MONO_VLSB);
    /**
     * @brief Destroy the UIWidget object
     */
    ~WidgetText();

    /// @brief initialise the link to the UIModelObject
    /// @param displayed_object
    void set_actual_displayed_object(UIModelObject *displayed_object);
    /**
     * @brief  add sub_widget to the current widget
     *
     * @param _sub_widget
     */
    void add_widget(WidgetText *_sub_widget);
    /**
     * @brief (re)draw the graphical elements of the widget.
     *
     * To save running time, we can (re)draw the widget only if the associated UIModelObject has_changed.
     *
     * Guidance to implement this function:
     *
     * - First: Scan all contained sub-widgets if any and call draw_refresh() member function of each of them.
     *
     * - then: update widget status according to the values of interest in the UIModelObject
     *
     * - refresh blinking if needed
     *
     * - Then: check if any changes in the model require a screen redraw
     *
     * - if redraw() required , execute the effective widget drawing (can be a private member function)
     * - and finally : clear model change flag if needed.
     *
     *        WARNING : When several widget display one Model, only the last one must clear_change_flag()
     */
    virtual void draw_refresh() = 0;
};
