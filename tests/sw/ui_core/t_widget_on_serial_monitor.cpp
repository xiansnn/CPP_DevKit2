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
#include <sstream>
#include <string>

#include "t_controlled_value.cpp"
#include "t_manager.cpp"

#include "sw/ui_core/ui_core.h"
#include "sw/widget/widget.h"
#include "sw/display_device/display_device.h"
#include "utilities/probe/probe.h"

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

std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

struct_ConfigGraphicWidget default_cfg{
    .pixel_frame_width = 100,
    .pixel_frame_height = 8,
    .fg_color = ColorIndex::WHITE,
    .bg_color = ColorIndex::BLACK};

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
    if (((MyIncrementalValueModel *)this->actual_displayed_model)->has_changed())
    {
        pr_D1.hi();
        //====get_value_of_interest
        std::string name = ((MyIncrementalValueModel *)this->actual_displayed_model)->get_name();
        int value = ((MyIncrementalValueModel *)this->actual_displayed_model)->get_value();
        ControlledObjectStatus model_status = ((MyIncrementalValueModel *)actual_displayed_model)->get_status();
        std::string status = status_to_string[model_status];

        //====draw
        switch (model_status)
        {
        case ControlledObjectStatus::IS_WAITING:
            sprintf(((PrinterDevice *)this->display_screen)->text_buffer,
                    "[%s] %s with value=%d\n",
                    name.c_str(), status.c_str(), value);
            break;
        case ControlledObjectStatus::HAS_FOCUS:
            sprintf(((PrinterDevice *)this->display_screen)->text_buffer,
                    "[%s] %s with value=%d\n",
                    name.c_str(), status.c_str(), value);
            break;
        case ControlledObjectStatus::IS_ACTIVE:
            sprintf(((PrinterDevice *)this->display_screen)->text_buffer,
                    "[%s] %s with value= %d %*c\n",
                    name.c_str(), status.c_str(), value, value_to_char_position(), '|');
            break;
        default:
            break;
        }
        //====show
        ((PrinterDevice *)this->display_screen)->show();
        //====clear change_flag
        this->actual_displayed_model->draw_widget_done();
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

void MyManagerWidget::draw()
{
    if (((MyManager *)this->actual_displayed_model)->has_changed())
    {
        pr_D4.hi();
        //====get_value_of_interest
        std::string text = "manager " + status_to_string[((MyIncrementalValueModel *)actual_displayed_model)->get_status()] + " with value=" +
                           std::to_string(((MyIncrementalValueModel *)actual_displayed_model)->get_value()) + "\n";
        //====draw
        sprintf(((PrinterDevice *)this->display_screen)->text_buffer, text.c_str());
        //====show
        ((PrinterDevice *)this->display_screen)->show();
        //====clear change_flag
        this->actual_displayed_model->draw_widget_done();

        pr_D4.lo();
    }
}
