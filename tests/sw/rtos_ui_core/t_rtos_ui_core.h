#pragma once
#include "sw/ui_core/ui_control_event.h"
#include "sw/ui_core/ui_core.h"
#include "utilities/probe/probe.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <set>


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

class my_Widget : public Widget
{
private:
    /* data */
public:
    my_Widget(DisplayDevice *display_device, Model *actual_displayed_model = nullptr);
    ~my_Widget();
    void draw();
};

class my_ProbePrinter : public DisplayDevice
{
private:
    Probe *probe;

public:
    void show();
    void show_from_display_queue(struct_DataToShow received_data_to_show);
    void clear_device_screen_buffer();
    my_ProbePrinter(Probe *probe);
    ~my_ProbePrinter();
};
