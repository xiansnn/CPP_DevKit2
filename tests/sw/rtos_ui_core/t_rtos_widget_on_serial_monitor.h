#pragma once

#include <sstream>
#include <string>

#include "t_rtos_controlled_value.h"
#include "t_rtos_manager.h"

#include "sw/ui_core/rtos_ui_core.h"
#include "sw/widget/widget.h"
#include "sw/display_device/display_device.h"
#include "utilities/probe/probe.h"


/// @brief This is an implementation of a pseudo-widget for test_ui_core program.
/// It write status and value of test_IncrementalValue on the serial monitor
class my_IncrementalValueWidgetOnSerialMonitor : public PrintWidget
{
private:
    float char_position_slope;
    float char_position_offset;
    uint8_t max_line_width = 21;
    int value_to_char_position();

public:
    /// @brief Construct a new Test Cursor Widget With Incremental Value object
    /// @param _actual_displayed_object
    my_IncrementalValueWidgetOnSerialMonitor(PrinterDevice *my_printer, my_IncrementalValueModel *_actual_displayed_object);

    ~my_IncrementalValueWidgetOnSerialMonitor();

    void draw();
};

/// @brief This is an implementation of a pseudo-widget for test_ui_core program.
/// It write status and value of MyManager on the serial monitor
class my_ManagerWidget : public PrintWidget
{
private:
public:
    /// @brief Construct a new MyManagerWidget object
    /// @param line_printer
    /// @param manager
    my_ManagerWidget(PrinterDevice *my_printer, my_TestManager *manager);

    ~my_ManagerWidget();

    void draw();
};













