/**
 * @file t_rtos_widget_on_serial_monitor.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include <sstream>
#include <string>

#include "t_rtos_controlled_value.h"
#include "t_rtos_manager.h"

#include "sw/ui_core/rtos_ui_core.h"
#include "sw/widget/rtos_widget.h"
#include "sw/display_device/display_device.h"
#include "utilities/probe/probe.h"


/// @brief This is an implementation of a pseudo-widget for test_ui_core program.
/// It write status and value of test_IncrementalValue on the serial monitor
class my_IncrementalValueWidgetOnSerialMonitor : public rtos_PrintWidget
{
private:
    float char_position_slope;
    float char_position_offset;
    uint8_t max_line_width = 21;
    int value_to_char_position();

public:
    /// @brief Construct a new Test Cursor Widget With Incremental Value object
    /// @param _actual_displayed_object
    my_IncrementalValueWidgetOnSerialMonitor(TerminalConsole *my_printer, my_IncrementalValueModel*_actual_displayed_object);

    ~my_IncrementalValueWidgetOnSerialMonitor();
    void send_text_to_DisplayGateKeeper(QueueHandle_t text_buffer_queue, SemaphoreHandle_t data_sent);

    void draw();
};

/// @brief This is an implementation of a pseudo-widget for test_ui_core program.
/// It write status and value of MyManager on the serial monitor
class my_ManagerWidgetOnSerialMonitor : public rtos_PrintWidget
{
private:
public:
    /// @brief Construct a new MyManagerWidget object
    /// @param line_printer
    /// @param manager
    my_ManagerWidgetOnSerialMonitor(TerminalConsole *my_printer, rtos_UIModelManager *manager);

    ~my_ManagerWidgetOnSerialMonitor();
    void send_text_to_DisplayGateKeeper(QueueHandle_t text_buffer_queue, SemaphoreHandle_t data_sent);

    void draw();
};













