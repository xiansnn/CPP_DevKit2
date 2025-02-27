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

class Model;

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
 * e.g. [final_type] * actual_displayed_model; of the actual [final_type] implementation of Model. This can be initialised by the constructor.
 *
 *
 */
class Widget
{
private:
    /// @brief store the value of the previous blinking phase.should be 0 or 1.
    int8_t previous_blinking_phase;

protected:
    /// @brief a pointer to the Model actually displayed by the widget
    Model *actual_displayed_model = nullptr;

    /// @brief ask if the blinking phase has changed
    /// \return true if phase has changed
    bool blinking_phase_has_changed();

    /// @brief The period of the blinking, in microseconds
    uint32_t blink_period_us;

    /// @brief A widget can be composed by several widget.
    std::vector<Widget *> widgets;

    virtual void get_value_of_interest() = 0;

public:
    /**
     * @brief Construct a new Widget object
     *
     * @param actual_displayed_model a pointer to the Model actually displayed by the widget.
     * Can be a nullptr if the widget doesn't need a Model (e.g. a pure cosmetic widget)
     * @param widget_anchor_x location in x of the widget within the hosting framebuffer
     * @param widget_anchor_y location in y of the widget within the hosting framebuffer
     */
    Widget(Model *actual_displayed_model, uint8_t widget_anchor_x, uint8_t widget_anchor_y);
    ~Widget();

    /// @brief location in x of the widget within the hosting framebuffer
    uint8_t widget_anchor_x;

    /// @brief location in y of the widget within the hosting framebuffer
    uint8_t widget_anchor_y;

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
    void add_widget(Widget *_sub_widget);

    /// @brief a pure virtual member that is called by draw_refresh method
    virtual void draw() = 0;

    /// @brief draw a rectangle around the widget.
    ///  \note As the border is a rectangle with fill=false, the border width can only be 1 pixel.
    /// @param color the color of the border
    virtual void draw_border(PixelColor color = PixelColor::WHITE) = 0;
};

/**
 * @brief the widget dedicated to graphics
 *
 */
class GraphicWidget : public Widget, public GraphicFramebuffer
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

public:
    virtual void draw_border(PixelColor color = PixelColor::WHITE);

    /**
     * @brief A short way to call GraphicDisplayDevice::show(pixel_buffer, anchor x, anchor y)
     *
     */
    void show();
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
                  Model *displayed_object,
                  struct_ConfigGraphicFramebuffer graph_cfg,
                  uint8_t widget_anchor_x,
                  uint8_t widget_anchor_y,
                  bool widget_with_border);
    /**
     * @brief Destroy the Widget object
     */
    ~GraphicWidget();
};

/**
 * @brief a dedicated class for text frame only
 *
 */
class TextWidget : public Widget, public TextFramebuffer
{
private:
protected:
    /// @brief if true, the widget is surrounded by a one-pixel border
    bool widget_with_border{false};

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
               Model *displayed_model,
               uint8_t widget_anchor_x,
               uint8_t widget_anchor_y,
               bool widget_with_border);
    ~TextWidget();

    /**
     * @brief A short way to call GraphicDisplayDevice::show(pixel_buffer, anchor x, anchor y)
     *
     */
    void show();

    /**
     * @brief we need draw() to be compliant with the pure virtual draw() inherited from Widget.
     *The draw() member call the show() member automatically.
     * Useful when we mix graphic and text widget in a UI environment.
     * Otherwise, we can use directly the TextFramebuffer::write(), but in this case we have to call draw_border(), then show() member.
     *
     */
    void draw();
    /**
     * @brief draw a one-pixel width around the the frame
     * \note This border can overwrite the characters!
     * To be improve with the use of pixel frame memory not based on byte page such as the OLED SSD1306.
     *
     * @param color
     */
    void draw_border(PixelColor color = PixelColor::WHITE);

    void get_value_of_interest();
};

/**
 * @brief  A widget used when we need to simply print but still want to take advantage of the status change management.
 *
 */
class PrintWidget
{
private:
protected:
    /// @brief a pointer to the Model actually displayed by the widget
    Model *actual_displayed_model = nullptr;
    /// @brief A widget can be composed by several widget.
    std::vector<PrintWidget *> widgets;

public:
    /// @brief a pointer to the printer display device
    PrinterDevice *display_device;

    /**
     * @brief Construct a new Dummy Widget object
     *
     * @param display_device the pointer to the printer display device
     * @param actual_displayed_model the pointer to the displayed model
     */
    PrintWidget(PrinterDevice *display_device, Model *actual_displayed_model);
    ~PrintWidget();

    void add_widget(PrintWidget* widget);

    virtual void print_refresh() = 0;
};
