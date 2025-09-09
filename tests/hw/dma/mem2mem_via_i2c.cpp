#include <stdio.h>
#include <string.h>

#include "hw/dma/hw_dma.h"

#include "hw/i2c/rtos_hw_i2c.h"

#include "utilities/probe/probe.h"

// #define PRINTF

Probe pr_D0 = Probe(0);
Probe pr_D1 = Probe(1);
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
// Probe pr_D6 = Probe(6);
// Probe pr_D7 = Probe(7);

#define MAX_DATA_SIZE 32
uint8_t write_data[MAX_DATA_SIZE];

TaskHandle_t periodic_data_generation_task_handle;
TaskHandle_t display_receive_data_task_handle;

QueueHandle_t i2c_rx_data_queue = xQueueCreate(8, sizeof(struct_RX_DataQueueI2C));

uint channel_rx;

void i2c_tx_fifo_handler();
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);

struct_ConfigMasterI2C master_config{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE,
    .i2c_master_handler = i2c_tx_fifo_handler};

struct_ConfigSlaveI2C slave_config{
    .i2c = i2c1,
    .sda_pin = 14,
    .scl_pin = 15,
    .baud_rate = I2C_FAST_MODE,
    .slave_address = 0x15,
    .slave_memory_size = 256,
    .handler = i2c_slave_handler};

rtos_HW_I2C_Master master = rtos_HW_I2C_Master(master_config);
HW_I2C_Slave slave = HW_I2C_Slave(slave_config);

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void vPeriodic_data_generation_task(void *pxPeriod)
{
    uint32_t *period_ms = (uint32_t *)pxPeriod;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint8_t write_data[MAX_DATA_SIZE];
    struct_TX_DataQueueI2C data_to_show;
    data_to_show.write_data = write_data;

    for (uint8_t mem_address = 0;; mem_address = (mem_address + MAX_DATA_SIZE) % slave_config.slave_memory_size)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(period_ms));
        // pr_D6.hi();

        char write_msg[MAX_DATA_SIZE]{0};
        snprintf(write_msg, MAX_DATA_SIZE, "Hello, slave@0x%02X mem[0x%02X]", slave_config.slave_address, mem_address);
        data_to_show.write_data_length = strlen(write_msg);
        data_to_show.mem_address = mem_address;

        for (size_t i = 0; i < data_to_show.write_data_length; i++)
        {
            write_data[i] = (uint16_t)write_msg[i];
        }
        xQueueSend(master.i2c_tx_data_queue, &data_to_show, portMAX_DELAY);
#ifdef PRINTF
        printf("Write %d at 0x%02X: '%s'\t", data_to_show.write_data_length, mem_address, write_msg); //
#endif
        // pr_D6.lo();
    }
}

void vI2c_sending_task(void *param)
{
    struct_TX_DataQueueI2C data_to_send;
    struct_RX_DataQueueI2C data_to_receive;
    while (true)
    {
        xQueueReceive(master.i2c_tx_data_queue, &data_to_send, portMAX_DELAY);
        // pr_D7.hi();
        master.burst_byte_write(slave_config.slave_address, data_to_send);
        data_to_receive.mem_address = data_to_send.mem_address;
        data_to_receive.read_data_length = data_to_send.write_data_length;

        xQueueSend(i2c_rx_data_queue, &data_to_receive, portMAX_DELAY);
        // pr_D7.lo();
    }
}

void vDisplay_received_data_task(void *param)
{
    uint8_t read_data[MAX_DATA_SIZE];
    char read_msg[MAX_DATA_SIZE]{0};
    struct_RX_DataQueueI2C received_data;
    while (true)
    {
        xQueueReceive(i2c_rx_data_queue, &received_data, portMAX_DELAY);
        pr_D5.hi();
      
        master.burst_byte_read(slave_config.slave_address, received_data, read_data);

        memcpy(read_msg, read_data, received_data.read_data_length);
        uint8_t read_msg_len = strlen(read_msg);
#ifdef PRINTF
        printf("Read %d char at 0x%02X: '%s'\n", read_msg_len, received_data.mem_address, read_msg);
#endif
        pr_D5.lo();
    }
}

int main()
{
#ifdef PRINTF
    stdio_init_all();
    printf("Test I2C exchange \n");
#endif

    xTaskCreate(vIdleTask, "idle_task0", 256, &pr_D0, 0, NULL);
    xTaskCreate(vPeriodic_data_generation_task, "compute_i2c_data", 250, (void *)500, 2, &periodic_data_generation_task_handle);
    xTaskCreate(vI2c_sending_task, "send_i2c_data", 250, NULL, 4, &master.task_to_notify_when_fifo_empty);
    xTaskCreate(vDisplay_received_data_task, "receive_i2c_data", 250, NULL, 1, &display_receive_data_task_handle);
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }
    return 0;
}

void i2c_tx_fifo_handler()
{
    pr_D4.hi();
    master.i2c_tx_fifo_dma_isr();
    pr_D4.lo();
}
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    pr_D1.hi();
    slave.slave_isr(event);
    pr_D1.lo();
}