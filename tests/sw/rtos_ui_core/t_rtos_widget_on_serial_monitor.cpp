#include "t_rtos_widget_on_serial_monitor.h"
#include <map>

std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_IDLE, "IS_IDLE"},
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

struct_ConfigGraphicWidget default_cfg{
    .pixel_frame_width = 100,
    .pixel_frame_height = 8,
    .fg_color = ColorIndex::WHITE,
    .bg_color = ColorIndex::BLACK};

my_IncrementalValueWidgetOnSerialMonitor::my_IncrementalValueWidgetOnSerialMonitor(PrinterDevice *my_printer, my_IncrementalValueModel *_actual_displayed_object)
    : PrintWidget(my_printer, _actual_displayed_object) , rtos_Widget()
{
    UIControlledIncrementalValue *_actual_displayed_model = (UIControlledIncrementalValue *)this->actual_displayed_model;
    int max_value = _actual_displayed_model->get_max_value();
    int min_value = _actual_displayed_model->get_min_value();
    char_position_slope = (max_line_width - 1.) / (max_value - min_value);
    char_position_offset = 1 - char_position_slope * min_value;
}

my_IncrementalValueWidgetOnSerialMonitor::~my_IncrementalValueWidgetOnSerialMonitor()
{
}

void my_IncrementalValueWidgetOnSerialMonitor::send_text_to_DisplayGateKeeper(QueueHandle_t text_buffer_queue, SemaphoreHandle_t data_sent)
{
}

void my_IncrementalValueWidgetOnSerialMonitor::draw()
{
    my_IncrementalValueModel *_actual_displayed_model = (my_IncrementalValueModel *)this->actual_displayed_model;
    PrinterDevice *_display_device = (PrinterDevice *)this->display_device;

    //====get_value_of_interest
    std::string name = _actual_displayed_model->get_name();
    int value = _actual_displayed_model->get_value();
    ControlledObjectStatus model_status = _actual_displayed_model->get_status();
    std::string status = status_to_string[model_status];

    //====draw
    switch (model_status)
    {
    case ControlledObjectStatus::IS_WAITING:
        sprintf(_display_device->text_buffer,
                "[%s] %s with value=%d\n",
                name.c_str(), status.c_str(), value);
        break;
    case ControlledObjectStatus::HAS_FOCUS:
        sprintf(_display_device->text_buffer,
                "[%s] %s with value=%d\n",
                name.c_str(), status.c_str(), value);
        break;
    case ControlledObjectStatus::IS_ACTIVE:
        sprintf(_display_device->text_buffer,
                "[%s] %s with value= %d %*c\n",
                name.c_str(), status.c_str(), value, value_to_char_position(), '|');
        break;
    default:
        break;
    }
    //====show
    _display_device->show();
}

int my_IncrementalValueWidgetOnSerialMonitor::value_to_char_position()
{
    return (char_position_slope * ((my_IncrementalValueModel *)this->actual_displayed_model)->get_value() + char_position_offset);
}

my_ManagerWidgetOnSerialMonitor::my_ManagerWidgetOnSerialMonitor(PrinterDevice *my_printer, my_TestManager *_manager)
    : PrintWidget(my_printer, _manager)
{
}

my_ManagerWidgetOnSerialMonitor::~my_ManagerWidgetOnSerialMonitor()
{
}

void my_ManagerWidgetOnSerialMonitor::draw()
{
    my_TestManager *_actual_display_model = (my_TestManager *)this->actual_displayed_model;
    PrinterDevice *_display_device = (PrinterDevice *)this->display_device;
    //====get_value_of_interest
    std::string text = "manager " + status_to_string[_actual_display_model->get_status()] + " with value=" +
                       std::to_string(_actual_display_model->get_value()) + "\n";
    //====draw
    sprintf(_display_device->text_buffer, text.c_str());
    //====show
    _display_device->show();
}
