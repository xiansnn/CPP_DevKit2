#include "t_rtos_ui_core.h"


my_Widget::my_Widget(DisplayDevice *display_device, Model *actual_displayed_model)
    : Widget(actual_displayed_model, display_device)
{
}

my_Widget::~my_Widget()
{
}

void my_Widget::draw()
{

    ((my_ProbePrinter *)this->display_device)->show();
}

void my_ProbePrinter::show()
{
    for (size_t i = 0; i < 10000; i++)
    {
        this->probe->hi();
        this->probe->lo();
    }
}

void my_ProbePrinter::show_from_display_queue(struct_DataToShow received_data_to_show)
{
    show();
}

void my_ProbePrinter::clear_device_screen_buffer()
{
    this->probe->pulse_us(100);
}

my_ProbePrinter::my_ProbePrinter(Probe *probe)
    : DisplayDevice()
{
    this->probe = probe;
}

my_ProbePrinter::~my_ProbePrinter()
{
}

void my_Model::cyclic_computation(Probe *probe)
{
    for (size_t i = 0; i < 4000; i++)
    {
        probe->hi();
        probe->lo();
    }
}

my_Model::my_Model()
    : Model()
{
}

my_Model::~my_Model()
{
}
