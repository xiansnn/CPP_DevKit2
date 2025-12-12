#include "t_rtos_widget_on_serial_monitor.h"
#include <map>

std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_IDLE, "IS_IDLE"},
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

struct_ConfigGraphicWidget default_cfg{
    .canvas_width_pixel = 100,
    .canvas_height_pixel = 8,
    .canvas_foreground_color = ColorIndex::WHITE,
    .canvas_background_color = ColorIndex::BLACK};

my_IncrementalValueWidgetOnSerialMonitor::my_IncrementalValueWidgetOnSerialMonitor(rtos_TerminalConsole *my_printer, my_IncrementalValueModel *_actual_displayed_model)
    : rtos_PrintWidget(_actual_displayed_model, my_printer)
{
    int max_value = _actual_displayed_model->get_max_value();
    int min_value = _actual_displayed_model->get_min_value();
    char_position_slope = (max_line_width - 1.) / (max_value - min_value);
    char_position_offset = 1 - char_position_slope * min_value;
}

my_IncrementalValueWidgetOnSerialMonitor::~my_IncrementalValueWidgetOnSerialMonitor()
{
}

void my_IncrementalValueWidgetOnSerialMonitor::draw()
{
    this->get_value_of_interest();
    //====draw
    switch (model_status)
    {
    case ControlledObjectStatus::IS_WAITING:
        sprintf(this->text_buffer,
                "[%s] %s with value=%d\n",
                name.c_str(), status.c_str(), value);
        break;
    case ControlledObjectStatus::HAS_FOCUS:
        sprintf(this->text_buffer,
                "[%s] %s with value=%d\n",
                name.c_str(), status.c_str(), value);
        break;
    case ControlledObjectStatus::IS_ACTIVE:
        sprintf(this->text_buffer,
                "[%s] %s with value= %d %*c\n",
                name.c_str(), status.c_str(), value, value_to_char_position(), '|');
        break;
    default:
        break;
    }
}

void my_IncrementalValueWidgetOnSerialMonitor::get_value_of_interest()
{
    my_IncrementalValueModel *_actual_displayed_model = (my_IncrementalValueModel *)this->actual_rtos_displayed_model;

    this->name = _actual_displayed_model->get_name();
    this->value = _actual_displayed_model->get_value();
    this->model_status = _actual_displayed_model->get_rtos_status();
    this->status = status_to_string[model_status];
}

int my_IncrementalValueWidgetOnSerialMonitor::value_to_char_position()
{
    return (char_position_slope * ((my_IncrementalValueModel *)this->actual_rtos_displayed_model)->get_value() + char_position_offset);
}

my_ManagerWidgetOnSerialMonitor::my_ManagerWidgetOnSerialMonitor(rtos_TerminalConsole *my_printer, rtos_UIModelManager *manager)
    : rtos_PrintWidget(manager, my_printer)
{
}

my_ManagerWidgetOnSerialMonitor::~my_ManagerWidgetOnSerialMonitor()
{
}

void my_ManagerWidgetOnSerialMonitor::draw()
{
    TerminalConsole *_display_device = (TerminalConsole *)this->display_device;
    get_value_of_interest();
    std::string text = "manager " + status + " with value=" +
                       std::to_string(this->current_focus_index) + "\n";
    //====draw
    sprintf(this->text_buffer, text.c_str());
}

void my_ManagerWidgetOnSerialMonitor::get_value_of_interest()
{
    my_TestManager *_actual_displayed_model = (my_TestManager *)this->actual_rtos_displayed_model;
    this->current_focus_index = _actual_displayed_model->get_current_focus_index();
    this->status = status_to_string[_actual_displayed_model->get_rtos_status()];
}
