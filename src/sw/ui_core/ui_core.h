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
#pragma once

#include "pico/stdlib.h"
#include "sw/ui_core/ui_control_event.h"
#include "sw/display_device/display_device.h"
#include "sw/widget/widget.h"

#include <vector>
#include <map>
#include <string>
#include <set>

/// @brief the time out used by the UIModelManager that indicates there is no more UI_ModelObject Status change.
#define UI_MODEL_OBJECT_STATUS_TIME_OUT_us 3000000

class UIController;
class Widget;

/// @brief This is the Model abstract class of Model_View_Control design pattern.
///
/// It handles change_flag, a semaphore used to indicate that a redraw is required for all attached widgets.
///
/// The controller or any other entities that modify the model must set the change_flag
/// and the widget in charge of its screen representation must clear the change_flag
/// \ingroup model
class Model
{
private:
    /// @brief the time in microseconds since the last status has changed
    uint32_t last_change_time;

    /// @brief The semaphore used to trigger the actual drawing of the widget on the screen.
    /// It is set with the number of widget attached to this model. Doing so, each time an attached widget is refreshed, the the change_flag is decrement by 1.
    /// When all attached widget are done, the change_flag = 0 (i.e. false).
    int change_flag;

protected:
    /// @brief the set of widgets that are in charge of viewing this model.
    /// \note USAGE: Used to count the number of widget that need to be refreshed
    std::set<Widget *> attached_widgets;

public:
    /// @brief Construct the Model object
    Model();

    /// @brief Destroy the Model object
    ~Model();

    /// @brief add a new widget to the set of attached_widgets
    /// @param new_widget
    void update_attached_widgets(Widget *new_widget);

    /// @brief get the number of attached widgets
    /// @return
    int get_number_of_attached_widget();

    /// @brief get the change flag status
    /// @return true means the redraw is required
    bool has_changed();

    /// @brief Set the change flag object to true
    void set_change_flag();

    /// @brief Set the change flag object to false
    void draw_widget_done();

    /// @brief compute time since the last status change
    /// @return this time in microsecond
    uint32_t get_time_since_last_change();

    /// @brief update drawing for each attached widgets
    virtual void draw_refresh_all_attached_widgets();
};

/// @brief Class that adds UI Controller to the basic Model class
/// \ingroup model
class UIControlledModel : public Model
{
private:
    /// @brief The status of the model, indicating if it is waiting, active or just ahs focus (pointed by the object manager)
    ControlledObjectStatus status{ControlledObjectStatus::IS_WAITING};

    /// @brief A pointer to the controller of this model.
    UIController *current_controller{nullptr};

public:
    UIControlledModel(/* args */);
    ~UIControlledModel();
    /**
     * @brief check if the _new_status change is effective,
     * then change it and set the change_flag to true.
     * @param _new_status
     */
    bool update_status(ControlledObjectStatus _new_status);
    /**
     * @brief if _new_controller is different from the current controller, change the current controller associated to the Model.
     * the new controller has is member current_controlled_model also changed.
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
    virtual void process_control_event(struct_ControlEventData control_event) = 0;
};

/**
 * @brief The UIControlledIncrementalValue is a kind of Model that have special feature such as a value that can be incremented or decremented.
 * This value runs between a min_value and a max_value.
 *
 * The increment value is configurable. A is_wrappable flag indicates how the value behaves once min or max values are reached.
 *  \ingroup model
 */
class UIControlledIncrementalValue : public UIControlledModel
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
    virtual bool increment_value();
    /**
     * @brief  Substract "increment" to the current value.
     *
     */
    virtual bool decrement_value();
    /**
     * @brief Set value to _new_value, and clip the result to min or max value if needed.
     *
     * @param _new_value
     */
    bool set_clipped_value(int _new_value);
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
 * An UIModelManager is built from :
 *
 * - Model : It inherits of the status and is controlled by a UIController.
 *
 * - UIControlledIncrementalValue : It is associated with a value that represents the current managed Model under focus or active.
 * \ingroup model
 */
class UIModelManager : public UIControlledIncrementalValue
{
protected:
    /**
     * @brief  check if there is a time out either on the managed models or the manager itself.
     *
     * This means no action on focus control and active status control.
     *
     * NOTICE: this is usefull when controller use IRQ, because we cannot detect no action when no more IRQ are triggered (up to now)
     *
     * @param managed_object_status_time_out_us the time out value in microsecond. default to 3000000 (3seconds)
     * @return ControlledObjectStatusTimeOutReason
     */
    ControlledObjectStatusTimeOutReason check_time_out(uint32_t managed_object_status_time_out_us = UI_MODEL_OBJECT_STATUS_TIME_OUT_us);

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
     * @brief The list of managed objects
     *
     */
    std::vector<UIControlledModel *> managed_models;
    /**
     * @brief the reference to the current active model object
     *
     */
    UIControlledModel *current_active_model;

    /**
     * @brief change the status of model object under focus to IS_ACTIVE
     *
     */
    void make_managed_model_active();
    /**
     * @brief leave the current managed object and return control to the manager
     *
     */
    void make_manager_active();

    /**
     * @brief Construct a new UIModelManager object
     *
     * @param is_wrappable if true, the scan over managed object wrap.
     */
    UIModelManager(bool is_wrappable = false);
    /**
     * @brief Destroy the UIModelManager object
     *
     */
    ~UIModelManager();
    /**
     * @brief add a new Model to the list of managed objects.
     *
     * @param _new_model
     */
    void add_managed_model(UIControlledModel *_new_model);
};

/**
 * @brief UIController is the abstract class that hosts all controller object in the Model-View-Controll design pattern.
 * \ingroup control
 */
class UIController
{
protected:
public:
    /**
     * @brief The reference to the Model currently under control.
     */
    UIControlledModel *current_controlled_model{nullptr};
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
    void update_current_controlled_object(UIControlledModel *_new_controlled_object);
};
