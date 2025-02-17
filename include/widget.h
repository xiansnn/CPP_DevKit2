/**
 * @file widget.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "display_device.h"
#include "framebuffer.h"
#include "ui_core.h"
#include <vector>

/**
 * @brief A widget is a displayed object on a device screen. It inherits from all framebuffer features, giving it textual and graphical capabilities.
 *
 * Being a framebuffer, it is defined by a width and a height, line and column of text, and graphics.
 * It is located within the display device screen at an anchor point (x,y).
 *
 * IMPORTANT NOTICE 1:  The widget is effectively drawn if something has changed in the UIModelObejct it represents. This allows to save drawing processing time.
 * However there is a strong limitation : only the widget buffer is transered to the device GDDRAM, based of its specific addressing scheme.
 * As a result, if the widget is located such that the buffer is written across device pages, the contents of the overwritten pages is lost.
 * This is why the widget height and the widget_anchor_y must be multiple of 8. Doing so the widget buffer bytes do not ovewrite pixel outside the widget border.
 *
 * IMPORTANT NOTICE 2: The final widget implementation must know what actual model object it displays. This final implementation must have a member (can be private)
 * e.g. [final_type] * actual_displayed_model; of the actual [final_type] implementation of UIModelObject. This can be initialised by the constructor.
 *
 *
 */
class UIWidget
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

    /// @brief A widget can be composed by several widget.
    std::vector<UIWidget *> widgets;

public:
    /**
     * @brief Construct a new UIWidget object
     *
     * @param actual_displayed_model
     */
    UIWidget(UIModelObject *actual_displayed_model);
    ~UIWidget();
    /**
     * @brief Set the blink period in microseconds
     *
     * @param blink_period default to 1 second
     */
    void set_blink_us(uint32_t blink_period = 1000000);
    /**
     * @brief  add sub_widget to the current widget
     *
     * @param _sub_widget
     */
    void add_widget(UIWidget *_sub_widget);
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
     * - if redraw() required , execute the effective widget drawing including border if required (can be a private member function)
     * - and finally : clear model change flag if needed.
     *
     *        WARNING : When several widget display one Model, only the last one must clear_change_flag()
     */
    virtual void draw_refresh() = 0;
};

/**
 * @brief the widget dedicated to graphics
 * 
 */
class GraphicWidget : public UIWidget, public GraphicFramebuffer
{
private:
protected:
    /// @brief if true, the widget is surrounded by a one-pixel border
    bool widget_with_border{true};

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

    /// @brief this is the border size of the widget. 0 if no border, 1 if border
    uint8_t widget_border_width;

    /// @brief draw a rectangle around the widget.
    /// IMPORTANT NOTICE: as the border is a rectangle with fill=false, the border width can only be 1 pixel.
    /// @param c the color of the border
    void draw_border(PixelColor c = PixelColor::WHITE);

public:
    /// @brief location in x of the widget within the hosting framebuffer
    uint8_t widget_anchor_x;

    /// @brief location in y of the widget within the hosting framebuffer
    uint8_t widget_anchor_y;

    /**
     * @brief Construct a new GraphicWidget object
     *
     * @param graphic_display_screen The display device on which the widget is drawn.
     * @param displayed_object the displayed object of the widget
     * @param graph_cfg the configuration data structure of the graphic framebuffer
     * @param widget_anchor_x the horizontal position where the widget start on the device screen
     * @param widget_anchor_y the verticaThe flag that indicates whether the widget has a border or notl position where the widget start on the device screen
     * @param widget_with_border
     * \image html widget.png
     */
    GraphicWidget(GraphicDisplayDevice *graphic_display_screen,
                  UIModelObject *displayed_object,
                  struct_ConfigGraphicFramebuffer graph_cfg,
                  uint8_t widget_anchor_x,
                  uint8_t widget_anchor_y,
                  bool widget_with_border);
    /**
     * @brief Destroy the UIWidget object
     */
    ~GraphicWidget();
};

/**
 * @brief a dedicated class for text frame only
 *
 * \todo simplication expected. check usefulness of members
 *
 */
class TextWidget : public UIWidget, public TextualFrameBuffer
{
private:
protected:
    /// @brief if true, the widget is surrounded by a one-pixel border
    bool widget_with_border{true};

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

    /// @brief this is the border size of the widget. 0 if no border, 1 if border
    uint8_t widget_border_width;

    /* data */
public:
    /**
     * @brief Construct a new Text Widget object
     *
     * @param device The display device on which the widget is drawn
     * @param text_cfg the configuration data for the textual frame
     * @param displayed_model the displayed model of the widget
     * @param widget_anchor_x the horizontal position where the widget start on the device screen
     * @param widget_anchor_y the verticaThe flag that indicates whether the widget has a border or notl position where the widget start on the device screen
     * @param widget_with_border flag that indicates if the frame has a border
     */
    TextWidget(GraphicDisplayDevice *device,
               struct_ConfigTextFramebuffer text_cfg,
               UIModelObject *displayed_model,
               uint8_t widget_anchor_x,
               uint8_t widget_anchor_y,
               bool widget_with_border);
    ~TextWidget();

    /// @brief location in x of the widget within the hosting framebuffer
    uint8_t widget_anchor_x;

    /// @brief location in y of the widget within the hosting framebuffer
    uint8_t widget_anchor_y;

    void draw_refresh();
};

/**
 * @brief  A widget used when we need to simply print but still want to take advantage of the status change management.
 *
 */
class PrintWidget
{
private:
protected:
public:
    /// @brief a pointer to the printer display device
    PrinterDevice *display_device;
    /// @brief the pointer to the displayed model
    /// \note may require recast to the actual displayed object
    UIModelObject *actual_displayed_model;
    /// @brief A widget can be composed by several widget.
    std::vector<PrintWidget *> widgets;

    /**
     * @brief Construct a new Dummy Widget object
     *
     * @param display_device the pointer to the printer display device
     * @param actual_displayed_model the pointer to the displayed model
     */
    PrintWidget(PrinterDevice *display_device, UIModelObject *actual_displayed_model);
    ~PrintWidget();
    /**
     * @brief  add sub_widget to the current widget
     *
     * @param _sub_widget
     */
    void add_widget(PrintWidget *_sub_widget);

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
     * - if redraw() required , execute the effective widget drawing including border if required (can be a private member function)
     * - and finally : clear model change flag if needed.
     *
     *        \note WARNING when several widget display one Model, only the last one must clear_change_flag()
     */
    virtual void draw_refresh();

    /// @brief a pure virtual member that is called by draw_refresh method
    virtual void draw() = 0;
};
