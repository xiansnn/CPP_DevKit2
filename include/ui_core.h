/**
 * @file ui_core.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
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
 * (0) IS_WAITING
 * (1) HAS_FOCUS
 * (2) IS_ACTIVE
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

/**
 * @brief The list of reason of manager time out.
 * (0) NO_TIME_OUT
 * (1) MANAGER_INACTIVE
 * (3) FOCUS_LOST
 */
enum class ControlledObjectStatusTimeOutReason
{
    /// @brief no time out
    NO_TIME_OUT,
    /// @brief The object is inactive, nothing to do.
    MANAGER_INACTIVE,
    /// @brief The widget or object manager is pointing to this model
    FOCUS_LOST,
};

class UIController;

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
     * NOTICE: this is usefull when controller use IRQ, because we cannot detect no action when no more IRQ are triggered (up to now)
     * 
     * @param managed_object_status_time_out_us the time out value in microsecond. default to 3000000 (3seconds)
     */
    ControlledObjectStatusTimeOutReason check_time_out(uint32_t managed_object_status_time_out_us=UI_MODEL_OBJECT_STATUS_TIME_OUT_us);
    /**
     * @brief The list of managed objects
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
     * @brief Construct a new UIObjectManager object
     * 
     * @param is_wrappable if true, the scan over managed object wrap.
     */
    UIObjectManager(bool is_wrappable=false);
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

#endif // UI_CORE_H
