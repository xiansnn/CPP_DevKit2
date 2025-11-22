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
    TaskHandle_t task_handle;
    /// @brief link a new widget task to the model
    void update_attached_rtos_widget(rtos_Widget *linked_widget);
    /// @brief notify all linked widget tasks that the model has changed
    void notify_all_linked_widget_task();
    // virtual void draw_refresh_all_attached_widgets();
    /// @brief constructor for RTOS model
    rtos_Model();
    /// @brief destructor for RTOS model
    ~rtos_Model();
};

class rtos_UIControlledModel : public rtos_Model
{
protected:
    /// @brief The status of the model, indicating if it is waiting, active or just ahs focus (pointed by the object manager)
    ControlledObjectStatus rtos_status{ControlledObjectStatus::IS_WAITING};

public:
    QueueHandle_t control_event_input_queue;
    std::vector<rtos_UIControlledModel *> managed_rtos_models;
    rtos_UIControlledModel* current_active_rtos_model;
    rtos_UIControlledModel();
    ~rtos_UIControlledModel();
    void update_rtos_status(ControlledObjectStatus _new_status);
    ControlledObjectStatus get_rtos_status();
    virtual void process_control_event(struct_ControlEventData control_event) = 0;
};

class rtos_UIModelManager : public rtos_UIControlledModel
{
private:
    int current_focus_index = 0;
    bool is_wrapable;
    int max_focus_index;

protected:
    virtual void increment_focus();
    virtual void decrement_focus();

public:
    rtos_UIModelManager(bool is_wrapable = true);
    ~rtos_UIModelManager();
    size_t get_current_focus_index();
    void make_managed_rtos_model_active();
    void make_rtos_manager_active();
    void add_managed_rtos_model( rtos_UIControlledModel* new_model);
    void notify_current_active_managed_model(UIControlEvent _event);
};

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




