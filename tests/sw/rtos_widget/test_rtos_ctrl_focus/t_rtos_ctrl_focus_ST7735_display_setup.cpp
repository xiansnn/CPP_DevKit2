#include "t_rtos_ctrl_focus_ST7735_display_setup.h"
#include "utilities/probe/probe.h"

struct_ConfigMasterSPI cfg_spi = {
    .spi = spi1,
    .sck_pin = 10,
    .tx_pin = 11,
    .rx_pin = 12,
    .cs_pin = 13,
    .baud_rate_Hz = 10 * 1000 * 1000};

struct_ConfigST7735 cfg_st7735{
    .display_type = DEVICE_DISPLAY_TYPE,
    .backlight_pin = 5,
    .hw_reset_pin = 15,
    .dc_pin = 14,
    .rotation = DEVICE_DISPLAY_ROTATION};

struct_ConfigTextWidget ST7735_title_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 0,
    .widget_anchor_y = 64,
    .font = font_12x16,
    .auto_next_char = true};

struct_ConfigTextWidget ST7735_values_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 68,
    .widget_anchor_y = 64,
    .font = font_12x16};

struct_ConfigGraphicWidget ST7735_graph_config{
    .canvas_width_pixel = 128,
    .canvas_height_pixel = 56,
    .canvas_foreground_color = ColorIndex::YELLOW,
    .canvas_background_color = ColorIndex::RED,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};
    

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
