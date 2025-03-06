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

#include "probe.h"

/// @brief  3 probes are create to observe the time execution with a logic analyser
Probe pr_D1 = Probe(1);
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

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

    void print_refresh();
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

    void print_refresh();
};

/// @brief test the composite widget features
class MySetOfWidget : public PrintWidget
{
private:
public:
    MySetOfWidget(PrinterDevice *my_printer);
    ~MySetOfWidget();
    void print_refresh();
};

std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

struct_ConfigGraphicWidget default_cfg{
    .pixel_frame_width = 100,
    .pixel_frame_height = 8,
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

void MyIncrementalValueWidgetOnSerialMonitor::print_refresh()
{
    if (((MyIncrementalValueModel *)this->actual_displayed_model)->has_changed())
    {
        pr_D1.hi();
        std::string name = ((MyIncrementalValueModel *)this->actual_displayed_model)->get_name();
        int value = ((MyIncrementalValueModel *)this->actual_displayed_model)->get_value();
        ControlledObjectStatus model_status = ((MyIncrementalValueModel *)actual_displayed_model)->get_status();
        std::string status = status_to_string[model_status];

        switch (model_status)
        {
        case ControlledObjectStatus::IS_WAITING:
            sprintf(this->display_device->text_buffer,
                    "[%s] %s with value=%d\n",
                    name.c_str(), status.c_str(), value);
            break;
        case ControlledObjectStatus::HAS_FOCUS:
            sprintf(this->display_device->text_buffer,
                    "[%s] %s with value=%d\n",
                    name.c_str(), status.c_str(), value);
            break;
        case ControlledObjectStatus::IS_ACTIVE:
            sprintf(this->display_device->text_buffer,
                    "[%s] %s with value= %d %*c\n",
                    name.c_str(), status.c_str(), value, value_to_char_position(), '|');
            break;
        default:
            break;
        }
        this->display_device->show();
        ((MyIncrementalValueModel *)this->actual_displayed_model)->ack_widget_drawn();
        pr_D1.lo();
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

void MyManagerWidget::print_refresh()
{
    if (((MyManager *)this->actual_displayed_model)->has_changed())
    {
        pr_D4.hi();
        std::string text = "manager " + status_to_string[((MyIncrementalValueModel *)actual_displayed_model)->get_status()] + " with value=" +
                           std::to_string(((MyIncrementalValueModel *)actual_displayed_model)->get_value()) + "\n";
        sprintf(this->display_device->text_buffer,text.c_str());

        this->display_device->show();
        
        ((MyManager *)this->actual_displayed_model)->ack_widget_drawn();

        pr_D4.lo();
    }
}

MySetOfWidget::MySetOfWidget(PrinterDevice *my_printer)
    : PrintWidget(my_printer, nullptr)
{
}

MySetOfWidget::~MySetOfWidget()
{
}

void MySetOfWidget::print_refresh()
{
    for (auto &&w : widgets)
    {
        w->print_refresh();
    }
}

