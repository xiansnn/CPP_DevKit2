/**
 * @file rtos_ui_core.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-02
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "sw/ui_core/ui_control_event.h"
#include "sw/ui_core/ui_core.h"
#include "utilities/probe/probe.h"
#include "sw/widget/rtos_widget.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include <set>

/// @brief RTOS wrapper for Model class
/// \ingroup model
class rtos_Model
{
private:
    /// @brief the set of linked widget tasks
    std::set<rtos_Widget *> attached_rtos_widget;
    // std::set<Widget *> attached_widgets;

public:
    /// @brief the task handle of the model task
    TaskHandle_t task_handle;
    /// @brief link a new widget task to the model
    /// @param linked_widget    the widget to be linked
    void update_attached_rtos_widget(rtos_Widget *linked_widget);
    /// @brief notify all linked widget tasks that the model has changed
    void notify_all_linked_widget_task();
    // virtual void draw_refresh_all_attached_widgets();
    /// @brief constructor for RTOS model
    rtos_Model();
    /// @brief destructor for RTOS model
    ~rtos_Model();
};

/// @brief RTOS wrapper for UIControlledModel class
/// \ingroup model
class rtos_UIControlledModel : public rtos_Model
{
protected:
    /// @brief The status of the model, indicating if it is waiting, active or just ahs focus (pointed by the object manager)
    ControlledObjectStatus rtos_status{ControlledObjectStatus::IS_WAITING};

public:
    /// @brief FreeRTOS queue handle used to receive control events for this model
    QueueHandle_t control_event_input_queue;

    /// @brief A static vector containing all the managed rtos_UIControlledModel objects
    std::vector<rtos_UIControlledModel *> managed_rtos_models;

    rtos_UIControlledModel();
    ~rtos_UIControlledModel();
    /// @brief update the status of the controlled model
    /// @param _new_status the new status to be set
    void update_rtos_status(ControlledObjectStatus _new_status);

    /// @brief Get the current status of the controlled model
    /// @return The current status of the controlled model
    ControlledObjectStatus get_rtos_status();

    /// @brief process a control event received from the control event queue
    /// @param control_event the control event to be processed
    virtual void process_control_event(struct_ControlEventData control_event) = 0;
};
/// @brief RTOS wrapper for UIModelManager class
/// \ingroup model
class rtos_UIModelManager : public rtos_UIControlledModel
{
private:
    /// @brief The current focus index among the managed models
    int current_focus_index = 0;
    /// @brief If true, the focus index wraps around when incremented/decremented beyond the limits
    bool is_wrapable;
    /// @brief The maximum focus index based on the number of managed models
    int max_focus_index;

protected:
    /// @brief Increment the focus index
    virtual void increment_focus();
    /// @brief Decrement the focus index
    virtual void decrement_focus();

public:
    /// @brief Construct a new rtos UIModel Manager object
    /// @param is_wrapable
    rtos_UIModelManager(bool is_wrapable = true);
    ~rtos_UIModelManager();

    /// @brief A static pointer to the current active rtos_UIControlledModel object
    rtos_UIControlledModel *current_active_rtos_model;

    /// @brief  Get the current focus index
    /// @return The current focus index
    size_t get_current_focus_index();

    /// @brief Make the current active managed rtos_UIControlledModel the active one
    void make_managed_rtos_model_active();

    /// @brief Make the rtos_UIModelManager the active one
    void make_rtos_manager_active();
    /// @brief Add a new managed rtos_UIControlledModel
    /// @param new_model The new model to be added
    void add_managed_rtos_model(rtos_UIControlledModel *new_model);
    /// @brief Forward a control event to the current active managed rtos_UIControlledModel
    /// @param control_event The control event to be forwarded
    void forward_control_event_to_active_managed_model(struct_ControlEventData *control_event);
};
/// @brief A model that holds an integer value that can be incremented or decremented within a defined range
class core_IncrementControlledModel
{
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
    core_IncrementControlledModel(int min_value = 0, int max_value = 10, bool is_wrappable = false, int increment = 1);
    ~core_IncrementControlledModel();

    /// @brief Increment the current value by the increment amount
    /// @return true if value changed, false if clipped and no change
    virtual bool increment_value();

    /// @brief  Decrement the current value by the increment amount
    /// @return true if value changed, false if clipped and no change
    virtual bool decrement_value();

    /// @brief Set value to _new_value, and clip the result to min or max value if needed.
    /// @param _new_value
    /// @return true if value changed, false if clipped and no change
    bool set_clipped_value(int _new_value);

    /// @brief Get the current value
    /// @return The current value
    int get_value();

    /// @brief Get the min value object
    /// @return int
    int get_min_value();

    /// @brief Get the max value object
    /// @return int
    int get_max_value();
};
