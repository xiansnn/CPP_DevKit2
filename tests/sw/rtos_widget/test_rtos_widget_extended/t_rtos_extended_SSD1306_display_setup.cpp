#include "t_rtos_extended_SSD1306_display_setup.h"
#include "utilities/probe/probe.h"

void I2C_display_gate_keeper_task(void *probe)
{
    struct_WidgetDataToGateKeeper received_data_to_show;

    while (true)
    {
        xQueueReceive(I2C_display_gate_keeper.graphic_widget_data, &received_data_to_show, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        I2C_display_gate_keeper.receive_widget_data(received_data_to_show);
        if (probe != NULL)
            ((Probe *)probe)->lo();
    }
}

void i2c_irq_handler()
{
    i2c_master.i2c_dma_isr();
};