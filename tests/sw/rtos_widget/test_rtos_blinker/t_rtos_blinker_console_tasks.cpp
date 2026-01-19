#include "t_rtos_blinker_console_tasks.h"
#include "utilities/probe/probe.h"


void clock_controller_console_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_focus_manager_console_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
    }
}

void main_clock_console_widget_task(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_main_clock_console_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
    }
}