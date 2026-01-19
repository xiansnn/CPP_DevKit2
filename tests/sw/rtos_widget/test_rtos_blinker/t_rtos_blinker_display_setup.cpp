#include "t_rtos_blinker_display_setup.h"
#include "utilities/probe/probe.h"

// ############ ST7735 TFT COlor display #######################################
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
// ############# SSD1306 OLED left and right display ######################################

struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE,
    .i2c_tx_master_handler = i2c_irq_handler};

struct_ConfigSSD1306 cfg_left_screen{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

struct_ConfigSSD1306 cfg_right_screen{
    .i2c_address = 0x3D,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

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