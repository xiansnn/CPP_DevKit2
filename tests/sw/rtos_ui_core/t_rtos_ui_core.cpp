#include "t_rtos_ui_core.h"

void rtos_Model::link_widget(rtos_Widget *linked_widget)
{
    this->linked_widget.insert(linked_widget);
}

void rtos_Model::notify_all_linked_widget()
{
    for (auto &&widget : linked_widget)
    {
        xTaskNotifyGive(widget->task_handle);
    }
}

rtos_Model::rtos_Model(Model *model)
{
    this->model = model;
}

rtos_Model::~rtos_Model()
{
}

rtos_Widget::rtos_Widget(Widget *widget)
{
    this->widget = widget;
}

rtos_Widget::~rtos_Widget()
{
}

My_Widget::My_Widget(PrinterDevice *display_device, Model *actual_displayed_model)
    : PrintWidget(display_device, actual_displayed_model)
{
}

My_Widget::~My_Widget()
{
}

void My_Widget::draw()
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

my_ProbePrinter::my_ProbePrinter(Probe *probe)
    : PrinterDevice(100, 1)
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
