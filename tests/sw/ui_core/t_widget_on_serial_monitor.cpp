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

#include "t_widget_on_serial_monitor.h"

#include "utilities/probe/probe.h"
extern Probe pr_D4 ;
extern Probe pr_D1 ;

std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

struct_ConfigGraphicWidget default_cfg{
    .canvas_width_pixel = 100,
    .canvas_height_pixel = 8,
    .canvas_foreground_color = ColorIndex::WHITE,
    .canvas_background_color = ColorIndex::BLACK};

MyIncrementalValueWidgetOnSerialMonitor::MyIncrementalValueWidgetOnSerialMonitor(TerminalConsole *my_printer, MyIncrementalValueModel *_actual_displayed_object)
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
            sprintf(((TerminalConsole *)this->display_device)->text_buffer,
                    "[%s] %s with value=%d\n",
                    name.c_str(), status.c_str(), value);
            break;
        case ControlledObjectStatus::HAS_FOCUS:
            sprintf(((TerminalConsole *)this->display_device)->text_buffer,
                    "[%s] %s with value=%d\n",
                    name.c_str(), status.c_str(), value);
            break;
        case ControlledObjectStatus::IS_ACTIVE:
            sprintf(((TerminalConsole *)this->display_device)->text_buffer,
                    "[%s] %s with value= %d %*c\n",
                    name.c_str(), status.c_str(), value, value_to_char_position(), '|');
            break;
        default:
            break;
        }
        //====show
        ((TerminalConsole *)this->display_device)->show();
        //====clear change_flag
        this->actual_displayed_model->draw_widget_done();
        pr_D1.lo();
    }
}

int MyIncrementalValueWidgetOnSerialMonitor::value_to_char_position()
{
    return (char_position_slope * ((MyIncrementalValueModel *)this->actual_displayed_model)->get_value() + char_position_offset);
}

MyManagerWidget::MyManagerWidget(TerminalConsole *my_printer, MyManager *_manager)
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
        sprintf(((TerminalConsole *)this->display_device)->text_buffer, text.c_str());
        //====show
        ((TerminalConsole *)this->display_device)->show();
        //====clear change_flag
        this->actual_displayed_model->draw_widget_done();

        pr_D4.lo();
    }
}
