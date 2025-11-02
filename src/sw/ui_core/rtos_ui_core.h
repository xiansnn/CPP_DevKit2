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

class rtos_Widget
{
private:
    struct_DataToShow data_to_display;

public:
    Widget *widget;
    TaskHandle_t task_handle;
    void send_image_to_DisplayGateKeeper(QueueHandle_t display_queue, SemaphoreHandle_t sending_done);
    void send_clear_device_command(QueueHandle_t display_queue, SemaphoreHandle_t sending_done);
    rtos_Widget(Widget *widget = nullptr);
    ~rtos_Widget();
};

class rtos_Model
{
private:
    std::set<rtos_Widget *> linked_widget_tasks;

public:
    Model *model;
    void link_widget(rtos_Widget *linked_widget);
    void notify_all_linked_widget_task();
    rtos_Model(Model *model = nullptr);
    ~rtos_Model();
};