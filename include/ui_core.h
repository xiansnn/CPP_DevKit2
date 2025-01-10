/**
 * @file ui_core.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#if !defined(UI_CORE_H)
#define UI_CORE_H

#include "pico/stdlib.h"
#include "framebuffer.h"
#include "ui_control_event.h"
#include "display_device.h"

#include <vector>
#include <map>
#include <string>

/// @brief the time out used by the UIObjectManager that indicates there is no more UI_ModelObject Status change.
#define UI_MODEL_OBJECT_STATUS_TIME_OUT_us 3000000


/**
 * @brief The list of status that a UIModelObject can have.
 */
enum class ControlledObjectStatus
{
    /// @brief The object is inactive, nothing to do.
    IS_WAITING,

    /// @brief The widget or object manager is pointing to this model
    HAS_FOCUS,

    /// @brief The user has selected (clicked) on this model. ControlEvent are then passed to this model in order to be processed.
    IS_ACTIVE
};

class UIController;

// /**
//  * @brief This is the abstract class to handle all generic behavior of physical display devices (e.g. OLED screen SSD1306).
//  * It derived from Framebuffer. This allows to draw text and graphics directly into the display framebuffer
//  * thanks to Framebuffet class text and graphic primitives indepently from any kind of widget
//  *
//  */
// class UIDisplayDevice : public Framebuffer
// {
// private:
// public:
//     /**
//      * @brief Construct a new UIDisplayDevice object
//      *
//      * @param width The width of physical screen, in pixel.
//      * @param height The height of physical screen, in pixel.
//      * @param format The framebuffer format ... see Framebuffer class FramebufferFormat enumeration
//      * @param txt_cnf A structure of data that reflect the framebuffer member related to text primitives
//      */
//     UIDisplayDevice(size_t width, size_t height, FramebufferFormat format = FramebufferFormat::MONO_VLSB, struct_FramebufferText txt_cnf = {.font = font_8x8});
//     /**
//      * @brief Destroy the UIDisplayDevice object
//      *
//      */
//     virtual ~UIDisplayDevice();
//     /**
//      * @brief This is an pure virtual member function that all final derived class must implement.
//      * It transfers the framebuffer buffer to the entire display screen buffer.
//      *
//      */
//     virtual void show() = 0;
//     /**
//      * @brief This is an pure virtual member function that all final derived class must implement.
//      * It transfers the framebuffer buffer to the a part of display screen buffer starting at the (anchor_x, anchor_y) coordinates of the screen , expressed in pixel.
//      *
//      * @param frame a pointer to the frame to be displayed
//      * @param anchor_x the x (horizontal)starting position of the frame within the display screen, (in pixel)
//      * @param anchor_y the y (vertical) starting position of the frame within the display screen, (in pixel)
//      */
//     virtual void show(Framebuffer *frame, uint8_t anchor_x, uint8_t anchor_y) = 0;
// };

/**
 * @brief This is the Model abstract class of Model_View_Control design pattern.
 *
 * It handles change_flag, a semaphore used to indicate that a screen draw_refresh is required.
 *
 *The controller or any other entities that modify the model must set the change_flag
 * and the widget in charge of its screen representation must clear the change_flag
 *
 */
class UIModelObject
{
private:
    /// @brief the time in microseconds since the last status has changed
    uint32_t last_change_time;

    /// @brief The semaphore used to trigger the actual drawing of the widget on the screen.
    bool change_flag{true};

    /// @brief The status of the model, indicating if it is waiting, active or just ahs focus (pointed by the object manager)
    ControlledObjectStatus status{ControlledObjectStatus::IS_WAITING};

    /// @brief A pointer to the controller of this model.
    UIController *current_controller{nullptr};

protected:
public:
    /// @brief Construct the UIModelObject object
    UIModelObject(/* args */);

    /// @brief Destroy the UIModelObject object
    ~UIModelObject();
    /**
     * @brief get the change flag status
     *
     * @return true means the redraw is required
     * @return false means the model is unchanged
     */
    bool has_changed();

    /// @brief Set the change flag object to true
    void set_change_flag();

    /// @brief Set the change flag object to false
    void clear_change_flag();

    /// @brief compute time since the last status change
    /// @return this time in microsecond
    uint32_t get_time_since_last_change();
    /**
     * @brief check if the _new_status change is effective, 
     * then change it and set the change_flag to true.
     * @param _new_status
     */
    void update_status(ControlledObjectStatus _new_status);
    /**
     * @brief if _new_controller is different from the current controller, change the current controller associated to the ModelObject.
     * the new controller has is member current_controlled_object also changed.
     * @param _new_controller
     */
    void update_current_controller(UIController *_new_controller);
    /**
     * @brief Get the status object
     *
     * @return ControlledObjectStatus
     */
    ControlledObjectStatus get_status();

    /**
     * @brief Get the current controller object.
     *
     * NOTICE: It is usually necessary to recast the abstract return UIController into the actual controller
     *
     * @return UIController*
     */
    UIController *get_current_controller();
    /**
     * @brief The purpose of this function is to implement the behavior of the implemented model object when a ControlEvent is received.
     *
     * @param _event
     */
    virtual void process_control_event(UIControlEvent _event) = 0;
};

/**
 * @brief The UIControlledIncrementalValue is a kind of UIModelObject that have special feature such as a value that can be incremented or decremented.
 * This value runs between a min_value and a max_value.
 *
 * The increment value is configurable. A is_wrappable flag indicates how the value behaves once min or max values are reached.
 *
 */
class UIControlledIncrementalValue : public UIModelObject
{
private:
protected:
    /// @brief The internal value incremented or decremented by action on the controller
    int value;
    /// @brief The maximum value that can be reached. Can be either negative or positive.
    int max_value;
    /// @brief The minimum value that can be reached. Can be either negative or positive.
    int min_value;
    /// @brief The number that is added or substracted to the current value. Default to 1.
    int increment;
    /// @brief If true, once the max (resp. min) value is reached, the next one wraps to min (resp*; max) value.
    /// If false values are clipped on min and max values.
    bool is_wrappable;

public:
    /**
     * @brief Construct a new UIControlledIncrementalValue object
     *
     * @param min_value   The minimum value that can be reached. Can be either negative or positive.
     * @param max_value   The maximum value that can be reached. Can be either negative or positive.
     * @param is_wrappable   If true, once the max (resp. min) value is reached, the next one wraps to min (resp*; max) value.
     * If false values are clipped on min and max values.
     * @param increment   The number that is added or substracted to the current value. Default to 1.
     */
    UIControlledIncrementalValue(int min_value = 0, int max_value = 10, bool is_wrappable = false, int increment = 1);
    /**
     * @brief Destroy the UIControlledIncrementalValue object
     *
     */
    ~UIControlledIncrementalValue();
    /**
     * @brief Add "increment" to the current value.
     *
     */
    virtual void increment_value();
    /**
     * @brief  Substract "increment" to the current value.
     *
     */
    virtual void decrement_value();
    /**
     * @brief Set value to _new_value, and clip the result to min or max value if needed.
     *
     * @param _new_value
     */
    void set_clipped_value(int _new_value);
    /**
     * @brief Get the value object
     *
     * @return int
     */
    int get_value();
    /**
     * @brief Get the min value object
     *
     * @return int
     */
    int get_min_value();
    /**
     * @brief Get the max value object
     *
     * @return int
     */
    int get_max_value();
};

/**
 * @brief This is an Abstract class that is used to implement the manager of object on a screen.
 *
 * An UIObjectManager is built from :
 *
 * - UIModelObject : It inherits of the status and is controlled by a UIController.
 *
 * - UIControlledIncrementalValue : It is associated with a value that represents the current managed UIModelObject under focus or active.
 *
 */
class UIObjectManager : public UIControlledIncrementalValue
{
protected:

    /**
     * @brief check if there is a time out either on the managed models or the manager itself.
     * 
     * This means no action on focus control and active status control.
     * 
     * @param time_out_us the time out value in microsecond. default to 3000000 (3seconds)
     */
    void check_time_out(uint32_t time_out_us=UI_MODEL_OBJECT_STATUS_TIME_OUT_us);
    /**
     * @brief The list of amaged objects
     *
     */
    std::vector<UIModelObject *> managed_models;
    /**
     * @brief the reference to the current active model object
     *
     */
    UIModelObject *current_active_model;
    /**
     * @brief change the status of model object under focus to IS_ACTIVE
     *
     */
    void make_managed_object_active();
    /**
     * @brief leave the current managed object and return control to the manager
     *
     */
    void make_manager_active();
    /**
     * @brief set focus on the next model in the list.
     *
     */
    virtual void increment_focus();
    /**
     * @brief set focus on the previous model in the list.
     *
     */
    virtual void decrement_focus();

public:
    /**
     * @brief create a new UIObjectManager.
     *
     */
    UIObjectManager(/* args */);
    /**
     * @brief Destroy the UIObjectManager object
     *
     */
    ~UIObjectManager();
    /**
     * @brief add a new UIModelObject to the list of managed objects.
     *
     * @param _new_model
     */
    void add_managed_model(UIModelObject *_new_model);
};

/**
 * @brief UIController is the abstract class that hosts all controller object in the Model-View-Controll design pattern.
 */
class UIController
{
protected:
public:
    /**
     * @brief The reference to the UIModelObject currently under control.
     */
    UIModelObject *current_controlled_object{nullptr};
    /**
     * @brief create a UIController object
     */
    UIController(/* args */);
    /**
     * @brief Destroy the UIController object
     */
    ~UIController();
    /**
     * @brief if the current controlled object is different from _new_controlled_object, change the current controlled object this new one.
     * By he same time, The controller of the new controlled object is updated.
     *
     * NOTICE: A controller can change its controlled object.
     * This is why it must know what is the current controlled object and it may be usefull that the controlled object know which is its controller.
     *
     * @param _new_controlled_object
     */
    void update_current_controlled_object(UIModelObject *_new_controlled_object);
};

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
 * e.g. [final_type] * actual_displayed_object; of the actual [final_type] implementation of UIModelObject. This can be initialised by the constructor.
 *
 *
 */
class UIWidget : public Framebuffer
{
private:
    /// @brief store the value of the previous blinking phase.should be 0 or 1.
    int8_t previous_blinking_phase;

protected:

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
    std::vector<UIWidget *> widgets;

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
    UIWidget(DisplayDevice *display_screen,
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
    ~UIWidget();
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
     * - if redraw() required , execute the effective widget drawing (can be a private member function)
     * - and finally : clear model change flag if needed.
     *
     *        WARNING : When several widget display one Model, only the last one must clear_change_flag()
     */
    virtual void draw_refresh() = 0;
};

#endif // UI_CORE_H
