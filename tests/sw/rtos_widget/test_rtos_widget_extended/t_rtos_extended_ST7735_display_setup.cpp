#include "t_rtos_extended_ST7735_display_setup.h"
#include "utilities/probe/probe.h"


void SPI_display_gate_keeper_task(void *probe)
{
    struct_WidgetDataToGateKeeper received_data_to_show;

    while (true)
    {
        xQueueReceive(SPI_display_gate_keeper.graphic_widget_data, &received_data_to_show, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        SPI_display_gate_keeper.receive_widget_data(received_data_to_show);
        if (probe != NULL)
            ((Probe *)probe)->lo();
    }
}
void end_of_TX_DMA_xfer_handler()
{
    spi_master.spi_tx_dma_isr();
}
