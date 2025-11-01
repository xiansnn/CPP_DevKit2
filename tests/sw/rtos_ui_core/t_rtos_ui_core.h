#pragma once
#include "sw/ui_core/ui_control_event.h"
#include "sw/ui_core/ui_core.h"
#include "utilities/probe/probe.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <set>

class rtos_Widget
{
private:
public:
    Widget *widget;
    TaskHandle_t task_handle;
    rtos_Widget(Widget *widget = nullptr);
    ~rtos_Widget();
};

class rtos_Model
{
private:
    std::set<rtos_Widget *> linked_widget;

public:
    Model *model;
    void link_widget(rtos_Widget *linked_widget);
    void notify_all_linked_widget();
    rtos_Model(Model *model = nullptr);
    ~rtos_Model();
};
//---------------------------------------------------------------------------------------------
class my_Model : public Model
{
private:
    /* data */
public:
    void cyclic_computation(Probe *probe);
    my_Model(/* args */);
    ~my_Model();
};

class My_Widget : public PrintWidget
{
private:
    /* data */
public:
    My_Widget(PrinterDevice *display_device, Model *actual_displayed_model = nullptr);
    ~My_Widget();
    void draw();
};

class my_ProbePrinter : public PrinterDevice
{
private:
    Probe *probe;

public:
    void show();
    my_ProbePrinter(Probe *probe);
    ~my_ProbePrinter();
};
