#pragma once
#include "sw/ui_core/ui_core.h"
#include "sw/widget/widget.h"
#include "t_controlled_value.h"
#include "t_manager.h"

/// @brief This is an implementation of a pseudo-widget for test_ui_core program.
/// It write status and value of test_IncrementalValue on the serial monitor
class MyIncrementalValueWidgetOnSerialMonitor : public PrintWidget
{
private:
    float char_position_slope;
    float char_position_offset;
    uint8_t max_line_width = 21;
    int value_to_char_position();

public:
    /// @brief Construct a new Test Cursor Widget With Incremental Value object
    /// @param _actual_displayed_object
    MyIncrementalValueWidgetOnSerialMonitor(TerminalConsole *my_printer, MyIncrementalValueModel *_actual_displayed_object);

    ~MyIncrementalValueWidgetOnSerialMonitor();

    void draw();
};

/// @brief This is an implementation of a pseudo-widget for test_ui_core program.
/// It write status and value of MyManager on the serial monitor
class MyManagerWidget : public PrintWidget
{
private:
public:
    /// @brief Construct a new MyManagerWidget object
    /// @param line_printer
    /// @param manager
    MyManagerWidget(TerminalConsole *my_printer, MyManager *manager);

    ~MyManagerWidget();

    void draw();
};


