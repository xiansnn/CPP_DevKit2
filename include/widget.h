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

#if !defined(WIDGET_H)
#define WIDGET_H

#include "display_device.h"
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
class Widget : public Framebuffer
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
    std::vector<Widget *> widgets;

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

    /**
     * @brief Construct a new UIWidget object
     *
     * @param display_screen   The display device on which the widget is drawn.
     * @param frame_width The width of the widget including the border
     * @param frame_height   the height of the widget including the border
     * @param widget_anchor_x  the horizontal position where the widget start on the device screen
     * @param widget_anchor_y  the vertical position where the widget start on the device screen
     * @param widget_with_border   The flag that indicates whether the widget has a border or not
     * @param widget_border_width   the width of the border. WARNING: can only be 1 pixel.
     * @param framebuffer_format  the addressing format of the actual display device
     * @param framebuffer_txt_cnf   a default textual configuration, with 8x8 font size
     *
     * \image html widget.png
     */
    Widget(DisplayDevice *display_screen,
           size_t frame_width,
           size_t frame_height,
           uint8_t widget_anchor_x,
           uint8_t widget_anchor_y,
           bool widget_with_border,
           uint8_t widget_border_width = 1,
           FramebufferFormat framebuffer_format = FramebufferFormat::MONO_VLSB,
           struct_FramebufferText framebuffer_txt_cnf = {.font = font_8x8});
    /**
     * @brief Destroy the UIWidget object
     */
    ~Widget();

    /// @brief initialise the link to the UIModelObject
    /// @param displayed_object 
    void set_actual_displayed_object(UIModelObject* displayed_object);
    /**
     * @brief  add sub_widget to the current widget
     *
     * @param _sub_widget
     */
    void add_widget(Widget *_sub_widget);
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

#endif // WIDGET_H
