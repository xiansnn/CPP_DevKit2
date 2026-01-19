#include "t_rtos_blinker_monitoring_tasks.h"
#include "utilities/probe/probe.h"

void controller_monitoring_widget_task(void *probe)
{
    I2C_display_gate_keeper.send_clear_device_command(&right_display);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        controller_monitoring_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        I2C_display_gate_keeper.send_widget_data(&controller_monitoring_widget);
    }
}

void clock_monitoring_widget_task(void *probe)
{
    I2C_display_gate_keeper.send_clear_device_command(&left_display);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        clock_monitoring_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        I2C_display_gate_keeper.send_widget_data(&clock_monitoring_widget);
    }
}