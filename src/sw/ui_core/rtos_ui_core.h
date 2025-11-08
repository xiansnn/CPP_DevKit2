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

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <set>

/// @brief RTOS wrapper for Widget class
/// \ingroup view
class rtos_Widget
{
private:
public:
    //// @brief pointer to the actual widget
    Widget *widget;
    /// @brief FreeRTOS task handle associated to the widget
    TaskHandle_t task_handle;
    /// @brief Send a command to clear the device screen buffer
    /// @param display_queue    the communication queue with the display gate keeper
    /// @param sending_done     the semaphore to signal when the sending is done
    void send_clear_device_command(QueueHandle_t display_queue, SemaphoreHandle_t sending_done);
    /// @brief constructor for RTOS widget
    rtos_Widget(Widget *widget = nullptr);
    /// @brief destructor for RTOS widget
    ~rtos_Widget();
};

/// @brief RTOS wrapper for Model class
/// \ingroup model
class rtos_Model
{
private:
    /// @brief the set of linked widget tasks
    std::set<rtos_Widget *> linked_widget_tasks;

public:
    /// @brief pointer to the actual model
    Model *model;
    /// @brief link a new widget task to the model
    void link_widget(rtos_Widget *linked_widget);
    /// @brief notify all linked widget tasks that the model has changed
    void notify_all_linked_widget_task();
    /// @brief constructor for RTOS model
    /// @param model    pointer to the actual model
    rtos_Model(Model *model = nullptr);
    /// @brief destructor for RTOS model
    ~rtos_Model();
};

class rtos_UIController
{
private:
    /* data */
public:
    UIController *controller;
    rtos_UIController(UIController *linked_controller);
    ~rtos_UIController();
};

class rtos_UIControlledModel : public rtos_Model
{
private:
    /* data */
public:
    rtos_UIControlledModel(UIControlledModel *linked_controlled_model);
    ~rtos_UIControlledModel();
    virtual void process_control_event_queue(UIControlEvent _event) = 0;
};

class rtos_UIControlledIncrementalValue : public rtos_UIControlledModel
{
private:
    /* data */
public:
    rtos_UIControlledIncrementalValue(UIControlledIncrementalValue *linked_ui_controlled_incremental_value);
    ~rtos_UIControlledIncrementalValue();
};

class rtos_UIModelManager : public rtos_UIControlledIncrementalValue
{
private:
    /* data */
public:
    rtos_UIModelManager(UIModelManager *linked_model_manager);
    ~rtos_UIModelManager();
    void make_managed_model_active();
    void make_manager_active();
};
