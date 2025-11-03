/**
 * @file test_rtos_ssd1306_device.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include <cstring>

#include "device/SSD1306/ssd1306.h"

#include "font/raspberry26x32.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p4 = Probe(4);
// Probe p5 = Probe(5);
// Probe p6 = Probe(6);
// Probe p7 = Probe(7);


#define INTER_TASK_DELAY 200

/// @brief  data structure used to pass multiple parameters to the RTOS task that handles the display update
struct struct_SSD1306DataToShow
{
    /// @brief pointer to the display device object
    GraphicDisplayDevice *display;
    /// @brief the area to be displayed
    struct_RenderArea display_area;
    /// @brief pointer to the data buffer to be displayed
    uint8_t *data_buffer;
    /// @brief the addressing mode to be used
    uint8_t addressing_mode = HORIZONTAL_ADDRESSING_MODE;
};

QueueHandle_t display_data_queue = xQueueCreate(8, sizeof(struct_SSD1306DataToShow));
SemaphoreHandle_t data_sent_to_I2C = xSemaphoreCreateBinary(); // synchro between display task and sending task

void i2c_irq_handler();

struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE,
    .i2c_tx_master_handler = i2c_irq_handler};

struct_ConfigSSD1306 cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

rtos_HW_I2C_Master master = rtos_HW_I2C_Master(cfg_i2c);
rtos_SSD1306 display = rtos_SSD1306(&master, cfg_ssd1306);

void i2c_irq_handler()
{
    master.i2c_dma_isr();
};

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

/**
 * @brief test contrast command.
 *
 * repeat 3 times [contrast 0, contrast 255, contrast 127]
 *
 * @param display
 */
void test_contrast(rtos_SSD1306 *display)
{
    p1.hi();
    display->clear_device_screen_buffer();

    struct_RenderArea area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    display->fill_GDDRAM_with_pattern(0x55, area);
    area = SSD1306::compute_render_area(32, 96, 16, 32);
    display->fill_GDDRAM_with_pattern(0xFF, area);

    for (size_t i = 0; i < 3; i++)
    {
        display->set_contrast(0);
        vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY * 2));
        display->set_contrast(255);
        vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY * 2));
        display->set_contrast(127);
        vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY * 2));
    }
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
};

/**
 * @brief  test addressing mode.
 * successsive test[horizontal addressing mode, verticale addressing mode, page addressing mode]
 *
 * @param display
 */
void test_addressing_mode(rtos_SSD1306 *display)
{
    struct_SSD1306DataToShow data_to_show;
    data_to_show.display = (rtos_SSD1306 *)display;
    struct_RenderArea area;
    static uint8_t image[128 * 8]{0x00};
    p1.hi();
    display->clear_device_screen_buffer();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));

    // memset(image, 0xFF, sizeof(image));

    // HORIZONTAL_ADDRESSING_MODE
    for (size_t i = 0; i < 4; i++)
    {
        memset(image, 0xAA, sizeof(image));
        area = SSD1306::compute_render_area(10 * i, 90 + 10 * i, 8 * i, 2 + 8 * i);
        data_to_show.display_area = area;
        data_to_show.data_buffer = image;
        data_to_show.addressing_mode = HORIZONTAL_ADDRESSING_MODE;
        xQueueSend(display_data_queue, &data_to_show, portMAX_DELAY);
        xSemaphoreTake(data_sent_to_I2C, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY / 2));
        display->clear_device_screen_buffer();
    }
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
    // VERTICAL_ADDRESSING_MODE
    for (size_t i = 0; i < 4; i++)
    {
        memset(image, 0xAA, sizeof(image));
        area = SSD1306::compute_render_area(40 + 10 * i, 50 + 10 * i, 8 * i, 30 + 8 * i);
        data_to_show.display = (rtos_SSD1306 *)display;
        data_to_show.display_area = area;
        data_to_show.data_buffer = image;
        data_to_show.addressing_mode = VERTICAL_ADDRESSING_MODE;
        xQueueSend(display_data_queue, &data_to_show, portMAX_DELAY);
        xSemaphoreTake(data_sent_to_I2C, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY / 2));
        display->clear_device_screen_buffer();
    }
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
    // PAGE_ADDRESSING_MODE
    for (size_t i = 0; i < 8; i++)
    {
        memset(image, 0x55, sizeof(image));
        area = SSD1306::compute_render_area(i * 10, 100 + i * 10, 8 * i, 8 * i);
        data_to_show.display = (rtos_SSD1306 *)display;
        data_to_show.display_area = area;
        data_to_show.data_buffer = image;
        data_to_show.addressing_mode = PAGE_ADDRESSING_MODE;
        xQueueSend(display_data_queue, &data_to_show, portMAX_DELAY);
        xSemaphoreTake(data_sent_to_I2C, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY / 2));
    }
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
};

/**
 * @brief test blink command
 *
 * @param display
 */
void test_blink(rtos_SSD1306 *display_device)
{
    static uint8_t image[SSD1306_BUF_LEN];
    struct_SSD1306DataToShow data_to_show;
    struct_RenderArea area;

    p1.hi();

    display_device->clear_device_screen_buffer();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY / 2));

    area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    display_device->fill_GDDRAM_with_pattern(0x81, area);
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY / 2));

    area = SSD1306::compute_render_area(64, 96, 15, 40);
    memset(image, 0x7E, area.buflen);
    data_to_show.display = (rtos_SSD1306 *)display_device;
    data_to_show.display_area = area;
    data_to_show.data_buffer = image;
    xQueueSend(display_data_queue, &data_to_show, portMAX_DELAY);
    xSemaphoreTake(data_sent_to_I2C, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));

    for (int i = 0; i < 2; i++)
    {
        display_device->set_all_pixel_ON();
        vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY / 4));
        display_device->set_display_from_RAM();
        vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY / 4));
    }
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
};
/**
 * @brief tst auto scrolling function of the SSD1306 device
 *
 * @param display
 */
void test_scrolling(rtos_SSD1306 *display)
{
    p1.hi();
    struct_SSD1306DataToShow data_to_show;
    display->clear_device_screen_buffer();
    // render 3 cute little raspberries
    struct_RenderArea area = SSD1306::compute_render_area(0, IMG_WIDTH - 1, 0, IMG_HEIGHT - 1);
    uint8_t offset = 5 + IMG_WIDTH; // 5px padding
    for (int i = 0; i < 3; i++)
    {
        // display->show_render_area(raspberry26x32, area);
        data_to_show.display = (rtos_SSD1306 *)display;
        data_to_show.display_area = area;
        data_to_show.data_buffer = raspberry26x32;
        xQueueSend(display_data_queue, &data_to_show, portMAX_DELAY);
        xSemaphoreTake(data_sent_to_I2C, portMAX_DELAY);

        area.start_col += offset;
        area.end_col += offset;
        data_to_show.display_area = area;
        data_to_show.data_buffer = raspberry26x32;
        xQueueSend(display_data_queue, &data_to_show, portMAX_DELAY);
        xSemaphoreTake(data_sent_to_I2C, portMAX_DELAY);
    }
    // start scrolling
    struct_ConfigScrollSSD1306 scroll_data = {
        .time_frame_interval = _25_FRAMES,
        .vertical_scrolling_offset = 1};
    display->horizontal_scroll(true, scroll_data);
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY * 4));
    display->horizontal_scroll(false, scroll_data);
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY * 4));
    display->vertical_scroll(true, scroll_data);
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY * 4));
    display->vertical_scroll(false, scroll_data);
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
};

void test_write_GDDRAM(void *display_device)
{
    p1.hi();
    struct_SSD1306DataToShow data_to_show;
    struct_RenderArea area;
    static uint8_t screen[SSD1306_BUF_LEN];
    uint8_t pattern = 0xF0;

    ((rtos_SSD1306 *)display_device)->clear_device_screen_buffer();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY / 2));

    ((rtos_SSD1306 *)display_device)->fill_GDDRAM_with_pattern(pattern, area);
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY / 2));

    pattern = 0xFF;
    area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    memset(screen, pattern, area.buflen);
    data_to_show.display = (rtos_SSD1306 *)display_device;
    data_to_show.display_area = area;
    data_to_show.data_buffer = screen;
    xQueueSend(display_data_queue, &data_to_show, portMAX_DELAY);
    xSemaphoreTake(data_sent_to_I2C, portMAX_DELAY);

    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
};

void display_gate_keeper_task(void *param)
{
    struct_SSD1306DataToShow received_data_to_show;

    while (true)
    {
        xQueueReceive(display_data_queue, &received_data_to_show, portMAX_DELAY);
        p4.hi();
        ((rtos_SSD1306 *)received_data_to_show.display)->show_render_area(received_data_to_show.data_buffer, 
                                        received_data_to_show.display_area, 
                                        received_data_to_show.addressing_mode);
        xSemaphoreGive(data_sent_to_I2C);
        p4.lo();
    }
}

void main_task(void *display_device)
{
    while (true)
    {
        test_write_GDDRAM((rtos_SSD1306 *)display_device);
        test_blink((rtos_SSD1306 *)display_device);
        test_contrast((rtos_SSD1306 *)display_device);        
        test_addressing_mode((rtos_SSD1306 *)display_device); 
        test_scrolling((rtos_SSD1306 *)display_device);
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(main_task, "main_task", 256, &display, 2, NULL);
    xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 256, NULL, 4, NULL);

    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
