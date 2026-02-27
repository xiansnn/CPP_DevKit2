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
/// This class represents the base model in the RTOS-based UI framework. It manages a set of linked widget tasks and provides mechanisms to notify them of changes.
/// The model is designed to be extended by more specific models that represent different types of data or functionality in the UI.
/// The rtos_Model class maintains a set of attached rtos_Widget pointers, which represent the widgets that are linked to this model. 
/// When the model changes, it can notify all linked widget tasks to update their display accordingly.
/// The rtos_Model class also includes a FreeRTOS task handle, which can be used to manage the model's own task if needed.
/// The rtos_UIControlledModel class extends rtos_Model and adds functionality for handling control events. 
/// It includes a FreeRTOS queue for receiving control events and a status to indicate whether the model is active, waiting, or has focus.
/// The rtos_UIModelManager class extends rtos_UIControlledModel and manages a collection of controlled models.
/// It provides functionality to manage focus among the controlled models and to forward control events to the currently active model. 
/// It also includes mechanisms for handling timeouts and managing the focus index among the controlled models.
/// The core_IncrementControlledModel class represents a model that holds an integer value that can be incremented or decremented within a defined range.
/// It includes functionality for wrapping values around the minimum and maximum limits if desired.
/// The core_CircularIncremetalControlledModel class extends core_IncrementControlledModel and is designed for representing circular quantities, such as angles.
/// It overrides the increment and decrement methods to wrap values around the defined range, making it suitable for use cases where values should loop back to the beginning after reaching a certain limit (e.g., 360 degrees wrapping back to 0).
/// \note The rtos_UIModelManager class is particularly useful for managing multiple controlled models and handling user interactions that may affect which model is currently active or has focus. 
/// \note The core_IncrementControlledModel and core_CircularIncremetalControlledModel classes provide basic functionality for managing incrementable values, which can be used in various UI components that require such behavior (e.g., sliders, counters, etc.).
/// \note The design of these classes allows for flexibility and extensibility, enabling developers to create complex UI models that can interact with various widgets and handle user input effectively in an RTOS environment.
/// \note The use of FreeRTOS features such as queues and task handles allows for efficient communication and task management within the UI framework, enabling responsive and interactive user interfaces even in resource-constrained environments.
/// \ingroup model
class rtos_Model
{
private:
    /// @brief the set of linked widget tasks
    /// \note This is a set of pointers to rtos_Widget objects that are linked to this model. When the model changes, it can notify all linked widget tasks to update their display accordingly.
    /// \note The use of a set ensures that each widget is only linked once and allows for efficient addition and removal of linked widgets.
    /// \note The rtos_Widget class represents a widget in the UI that can be linked to a model. When the model changes, it can call the notify_all_linked_widget_task() method to inform all linked widgets of the change, allowing them to update their display or behavior accordingly.
    /// \note The rtos_Model class can be extended by more specific model classes that represent different types of data or functionality in the UI. These specific model classes can then be linked to various widgets that display or interact with the model's data.
    std::set<rtos_Widget *> attached_rtos_widget;

public:
    /// @brief the task handle of the model task
    /// \note This is a FreeRTOS task handle that can be used to manage the model's own task if needed. It allows for operations such as suspending, resuming, or deleting the task associated with the model.
    /// \note The model's task can be responsible for managing the model's state, processing events, or performing periodic updates. By having a task handle, the model can interact with the FreeRTOS scheduler and manage its execution effectively within the RTOS environment.
    TaskHandle_t task_handle{nullptr};
    /// @brief link a new widget task to the model
    /// @param linked_widget    the widget to be linked
    void update_attached_rtos_widget(rtos_Widget *linked_widget);
    /// @brief notify all linked widget tasks that the model has changed
    void notify_all_linked_widget_task();
    /// @brief constructor for RTOS model
    rtos_Model();
    /// @brief destructor for RTOS model
    ~rtos_Model();
};

/// @brief RTOS wrapper for UIControlledModel class
/// This class represents a model that can be controlled by user interactions in the RTOS-based UI framework. It extends the rtos_Model class and adds functionality for handling control events and managing the status of the model (e.g., active, waiting, has focus).
/// The rtos_UIControlledModel class includes a FreeRTOS queue for receiving control events, which can be processed by the model to update its state or behavior based on user interactions.
/// The status of the model is represented by the ControlledObjectStatus enum, which indicates whether the model is idle, waiting, has focus, or is active. This status can be used by the UI framework to manage user interactions and determine which model should receive control events.
/// The rtos_UIControlledModel class is designed to be extended by more specific controlled model classes that represent different types of data or functionality in the UI. These specific controlled model classes can then be linked to various widgets that display or interact with the model's data, allowing for dynamic and interactive user interfaces in an RTOS environment.
/// \ingroup model
class rtos_UIControlledModel : public rtos_Model
{
protected:
    /// @brief The status of the model, indicating if it is waiting, active or just ahs focus (pointed by the object manager)
    ControlledObjectStatus rtos_status{ControlledObjectStatus::IS_WAITING};

public:
    /// @brief FreeRTOS queue handle used to receive control events for this model
    QueueHandle_t control_event_input_queue;

    rtos_UIControlledModel();
    ~rtos_UIControlledModel();
    /// @brief update the status of the controlled model
    /// @param _new_status the new status to be set
    void update_rtos_status(ControlledObjectStatus _new_status);

    /// @brief Get the current status of the controlled model
    /// @return The current status of the controlled model
    ControlledObjectStatus get_rtos_status();

    /// @brief process a control event received from the control event queue.
    /// @note This method is meant to be overridden by specific controlled model classes to define how they process control events. 
    /// The control event data includes the type of event and the GPIO number that triggered the event, allowing the model to respond appropriately based on user interactions.
    /// @param control_event the control event to be processed
    virtual void process_control_event(struct_ControlEventData control_event) = 0;
};
/// @brief RTOS wrapper for UIModelManager class
/// This class represents a manager that can manage multiple controlled models in the RTOS-based UI framework. It extends the rtos_UIControlledModel class and provides functionality for managing focus among the controlled models, forwarding control events to the currently active model, and handling timeouts.
/// The rtos_UIModelManager class maintains a collection of controlled models and manages the focus index among them. It provides methods to increment and decrement the focus index, which can be used to navigate through the controlled models. The manager can also forward control events to the currently active model, allowing for dynamic interaction based on user input.
/// The rtos_UIModelManager class is designed to be extended by more specific model manager classes that represent different types of model management functionality in the UI. These specific model manager classes can then be linked to various widgets that display or interact with the managed models, enabling complex and interactive user interfaces in an RTOS environment.
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

    /// @brief A static vector containing all the managed rtos_UIControlledModel objects
    std::vector<rtos_UIControlledModel *> managed_rtos_models;

    /// @brief A static pointer to the current active rtos_UIControlledModel object
    rtos_UIControlledModel *current_active_rtos_model;

    /// @brief  Process a time out condition for the manager
    /// @param manager   The manager to process the timeout for
    /// @param time_out_ms  The timeout in milliseconds
    void process_event_and_time_out_condition(rtos_UIModelManager *manager, uint time_out_ms);

    /// @brief  Get the current focus index
    /// @return The current focus index
    size_t get_current_focus_index();

    /// @brief Make the current active managed rtos_UIControlledModel the active one
    void make_managed_rtos_model_active();

    /// @brief Reset the current focus index to zero
    void reset_current_focus_index();

    /// @brief Make the rtos_UIModelManager the active one
    /// @note This will set the manager status to IS_ACTIVE leave set the previous active managed model to HAS_FOCUS
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

/// @brief A model that holds an integer value that can be incremented or decremented within a defined range in a circular manner.
/// For example, incrementing beyond the max value wraps around to the min value, and decrementing below the min value wraps around to the max value.
/// This is particularly useful for representing circular quantities such as angles (0 to 360 degrees). In such case 360 = 0.
class core_CircularIncremetalControlledModel : public core_IncrementControlledModel
{
private:
    /* data */
public:
    /// @brief  Construct a new core Circular Incremetal Controlled Model object
    /// @param increment        
    /// @param min_value 
    /// @param max_value 
    core_CircularIncremetalControlledModel(int increment = 1, int min_value = 0, int max_value = 360);
    ~core_CircularIncremetalControlledModel();

    /// @brief Increment the current value by the increment amount
    /// @return true if value changed, false if clipped and no change
    virtual bool increment_value();

    /// @brief  Decrement the current value by the increment amount
    /// @return true if value changed, false if clipped and no change
    virtual bool decrement_value();
};
