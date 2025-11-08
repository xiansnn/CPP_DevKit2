#include "t_rtos_widget_on_serial_monitor.h"
#include <map>

std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

struct_ConfigGraphicWidget default_cfg{
    .pixel_frame_width = 100,
    .pixel_frame_height = 8,
    .fg_color = ColorIndex::WHITE,
    .bg_color = ColorIndex::BLACK};



my_IncrementalValueWidgetOnSerialMonitor::my_IncrementalValueWidgetOnSerialMonitor(PrinterDevice *my_printer, my_IncrementalValueModel *_actual_displayed_object)
    : PrintWidget(my_printer, _actual_displayed_object)
{
    int max_value = ((UIControlledIncrementalValue *)this->actual_displayed_model)->get_max_value();
    int min_value = ((UIControlledIncrementalValue *)this->actual_displayed_model)->get_min_value();
    char_position_slope = (max_line_width - 1.) / (max_value - min_value);
    char_position_offset = 1 - char_position_slope * min_value;
}

my_IncrementalValueWidgetOnSerialMonitor::~my_IncrementalValueWidgetOnSerialMonitor()
{
}

void my_IncrementalValueWidgetOnSerialMonitor::draw()
{
    if (((my_IncrementalValueModel *)this->actual_displayed_model)->has_changed())
    {
    
        //====get_value_of_interest
        std::string name = ((my_IncrementalValueModel *)this->actual_displayed_model)->get_name();
        int value = ((my_IncrementalValueModel *)this->actual_displayed_model)->get_value();
        ControlledObjectStatus model_status = ((my_IncrementalValueModel *)actual_displayed_model)->get_status();
        std::string status = status_to_string[model_status];

        //====draw
        switch (model_status)
        {
        case ControlledObjectStatus::IS_WAITING:
            sprintf(((PrinterDevice *)this->display_device)->text_buffer,
                    "[%s] %s with value=%d\n",
                    name.c_str(), status.c_str(), value);
            break;
        case ControlledObjectStatus::HAS_FOCUS:
            sprintf(((PrinterDevice *)this->display_device)->text_buffer,
                    "[%s] %s with value=%d\n",
                    name.c_str(), status.c_str(), value);
            break;
        case ControlledObjectStatus::IS_ACTIVE:
            sprintf(((PrinterDevice *)this->display_device)->text_buffer,
                    "[%s] %s with value= %d %*c\n",
                    name.c_str(), status.c_str(), value, value_to_char_position(), '|');
            break;
        default:
            break;
        }
        //====show
        ((PrinterDevice *)this->display_device)->show();
        //====clear change_flag
        this->actual_displayed_model->draw_widget_done();

    }
}

int my_IncrementalValueWidgetOnSerialMonitor::value_to_char_position()
{
    return (char_position_slope * ((my_IncrementalValueModel *)this->actual_displayed_model)->get_value() + char_position_offset);
}

my_ManagerWidget::my_ManagerWidget(PrinterDevice *my_printer, my_TestManager *_manager)
    : PrintWidget(my_printer, _manager)
{
}

my_ManagerWidget::~my_ManagerWidget()
{
}

void my_ManagerWidget::draw()
{
    if (((my_TestManager *)this->actual_displayed_model)->has_changed())
    {
        //====get_value_of_interest
        std::string text = "manager " + status_to_string[((my_IncrementalValueModel *)actual_displayed_model)->get_status()] + " with value=" +
                           std::to_string(((my_IncrementalValueModel *)actual_displayed_model)->get_value()) + "\n";
        //====draw
        sprintf(((PrinterDevice *)this->display_device)->text_buffer, text.c_str());
        //====show
        ((PrinterDevice *)this->display_device)->show();
        //====clear change_flag
        this->actual_displayed_model->draw_widget_done();
    }
}
