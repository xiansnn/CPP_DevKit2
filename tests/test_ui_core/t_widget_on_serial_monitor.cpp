/**
 * @file t_widget_on_serial_monitor.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "t_controlled_value.cpp"
#include "t_manager.cpp"

#include "ui_core.h"
#include "widget.h"

#include "display_device.h"

#include <sstream>
#include <string>

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
    MyIncrementalValueWidgetOnSerialMonitor(PrinterDevice *my_printer, MyIncrementalValueModel *_actual_displayed_object);

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
    MyManagerWidget(PrinterDevice *my_printer, MyManager *manager);

    ~MyManagerWidget();

    void draw();
};

/// @brief test the composite widget features
class MySetOfWidget : public PrintWidget
{
private:
public:
    MySetOfWidget(PrinterDevice *my_printer);
    ~MySetOfWidget();
    void draw();
};

std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

struct_ConfigGraphicFramebuffer default_cfg{
    .frame_width = 100,
    .frame_height = 8,
    .fg_color = PixelColor::WHITE,
    .bg_color = PixelColor::BLACK};

MyIncrementalValueWidgetOnSerialMonitor::MyIncrementalValueWidgetOnSerialMonitor(PrinterDevice *my_printer, MyIncrementalValueModel *_actual_displayed_object)
    : PrintWidget(my_printer, _actual_displayed_object)
{
    int max_value = ((UIControlledIncrementalValue *)this->actual_displayed_model)->get_max_value();
    int min_value = ((UIControlledIncrementalValue *)this->actual_displayed_model)->get_min_value();
    char_position_slope = (max_line_width - 1.) / (max_value - min_value);
    char_position_offset = 1 - char_position_slope * min_value;
}

MyIncrementalValueWidgetOnSerialMonitor::~MyIncrementalValueWidgetOnSerialMonitor()
{
}

void MyIncrementalValueWidgetOnSerialMonitor::draw()
{
    switch (actual_displayed_model->get_status())
    {
    case ControlledObjectStatus::IS_WAITING:
        printf("[%s] %s with value=%d\n",
               ((MyIncrementalValueModel *)this->actual_displayed_model)->get_name().c_str(),
               status_to_string[actual_displayed_model->get_status()].c_str(),
               ((MyIncrementalValueModel *)this->actual_displayed_model)->get_value());
        break;
    case ControlledObjectStatus::HAS_FOCUS:
        printf("[%s] %s with value=%d\n",
               ((MyIncrementalValueModel *)this->actual_displayed_model)->get_name().c_str(),
               status_to_string[actual_displayed_model->get_status()].c_str(),
               ((MyIncrementalValueModel *)this->actual_displayed_model)->get_value());
        break;
    case ControlledObjectStatus::IS_ACTIVE:
        printf("[%s] %s with value= %d %*c\n",
               ((MyIncrementalValueModel *)this->actual_displayed_model)->get_name().c_str(),
               status_to_string[actual_displayed_model->get_status()].c_str(),
               ((MyIncrementalValueModel *)this->actual_displayed_model)->get_value(),
               value_to_char_position(), '|');
        break;
    default:
        break;
    }
}

int MyIncrementalValueWidgetOnSerialMonitor::value_to_char_position()
{
    return (char_position_slope * ((MyIncrementalValueModel *)this->actual_displayed_model)->get_value() + char_position_offset);
}

MyManagerWidget::MyManagerWidget(PrinterDevice *my_printer, MyManager *_manager)
    : PrintWidget(my_printer, _manager)
{
}

MyManagerWidget::~MyManagerWidget()
{
}

void MyManagerWidget::draw()
{
    std::string text = "manager " + status_to_string[actual_displayed_model->get_status()] + " with value=" +
                       std::to_string(((MyIncrementalValueModel *)actual_displayed_model)->get_value()) + "\n";
    printf(text.c_str());
}

MySetOfWidget::MySetOfWidget(PrinterDevice *my_printer)
    : PrintWidget(my_printer, nullptr)
{
}

MySetOfWidget::~MySetOfWidget()
{
}

void MySetOfWidget::draw()
{
}
