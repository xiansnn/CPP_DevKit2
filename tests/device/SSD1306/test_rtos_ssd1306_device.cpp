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
// #include <string.h>
// #include <string>
// #include <math.h>
// #include <numbers>
// #include <sstream>
// #include <iomanip>
#include <cstring>

#include "device/SSD1306/ssd1306.h"

// #include "font/raspberry26x32.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p4 = Probe(4);
// Probe p5 = Probe(5);
// Probe p6 = Probe(6);
// Probe p7 = Probe(7);

// #define DEGREE "\xF8"
#define INTER_TASK_DELAY 500

QueueHandle_t display_data_queue = xQueueCreate(8, sizeof(struct_SSD1306DataToShow));
SemaphoreHandle_t data_sent = xSemaphoreCreateBinary(); // synchro between display task and sending task

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

// HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
// SSD1306 display = SSD1306(&master, cfg_ssd1306);
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

// /**
//  * @brief test contrast command.
//  *
//  * repeat 3 times [contrast 0, contrast 255, contrast 127]
//  *
//  * @param display
//  */
// void test_contrast(SSD1306 *display)
// {
//     p1.hi();
//     display->clear_device_screen_buffer();
//     struct_RenderArea area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
//     display->fill_pattern_and_show_GDDRAM(0x55, area);
//     area = SSD1306::compute_render_area(32, 96, 16, 32);
//     display->fill_pattern_and_show_GDDRAM(0xFF, area);
//     for (size_t i = 0; i < 3; i++)
//     {
//         display->set_contrast(0);
//         sleep_ms(1000);
//         display->set_contrast(255);
//         sleep_ms(1000);
//         display->set_contrast(127);
//         sleep_ms(1000);
//     }
//     p1.lo();
// };

// /**
//  * @brief  test addressing mode.
//  * successsive test[horizontal addressing mode, verticale addressing mode, page addressing mode]
//  *
//  * @param display
//  */
// void test_addressing_mode(SSD1306 *display)
// {
//     p1.hi();
//     uint8_t image[128 * 8]{0x00};
//     memset(image, 0xFE, sizeof(image));
//     sleep_ms(1000);
//     display->clear_device_screen_buffer();
//     struct_RenderArea area;
//     // HORIZONTAL_ADDRESSING_MODE
//     for (size_t i = 0; i < 4; i++)
//     {
//         memset(image, 0xAA, sizeof(image));
//         area = SSD1306::compute_render_area(10 * i, 90 + 10 * i, 8 * i, 2 + 8 * i);
//         display->show_render_area(image, area, HORIZONTAL_ADDRESSING_MODE);
//         sleep_ms(1000);
//         display->clear_device_screen_buffer();
//     }
//     // VERTICAL_ADDRESSING_MODE
//     for (size_t i = 0; i < 4; i++)
//     {
//         memset(image, 0xAA, sizeof(image));
//         area = SSD1306::compute_render_area(40 + 10 * i, 50 + 10 * i, 8 * i, 30 + 8 * i);
//         display->show_render_area(image, area, VERTICAL_ADDRESSING_MODE);
//         sleep_ms(1000);
//         display->clear_device_screen_buffer();
//     }
//     // PAGE_ADDRESSING_MODE
//     for (size_t i = 0; i < 8; i++)
//     {
//         memset(image, 0x55, sizeof(image));
//         area = SSD1306::compute_render_area(i * 10, 100 + i * 10, 8 * i, 8 * i);
//         display->show_render_area(image, area, PAGE_ADDRESSING_MODE);
//         sleep_ms(1000);
//     }
//     p1.lo();
// };

/**
 * @brief test blink command
 *
 * @param display
 */
void test_blink(SSD1306 *display)
{
    struct_RenderArea area;
    p1.hi();
    display->clear_device_screen_buffer();
    area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    display->fill_pattern_and_show_GDDRAM(0x81, area);
    area = SSD1306::compute_render_area(64, 96, 15, 40);
    display->fill_pattern_and_show_GDDRAM(0x7E, area);
    for (int i = 0; i < 2; i++)
    {
        display->set_all_pixel_ON();
        sleep_ms(INTER_TASK_DELAY/4);
        display->set_display_from_RAM();
        sleep_ms(INTER_TASK_DELAY/4);
    }
    p1.lo();
};
// /**
//  * @brief tst auto scrolling function of the SSD1306 device
//  *
//  * @param display
//  */
// void test_scrolling(SSD1306 *display)
// {
//     p1.hi();
//     display->clear_device_screen_buffer();
//     // render 3 cute little raspberries
//     struct_RenderArea area = SSD1306::compute_render_area(0, IMG_WIDTH - 1, 0, IMG_HEIGHT - 1);
//     uint8_t offset = 5 + IMG_WIDTH; // 5px padding
//     for (int i = 0; i < 3; i++)
//     {
//         display->show_render_area(raspberry26x32, area);
//         area.start_col += offset;
//         area.end_col += offset;
//     }
//     // start scrolling
//     struct_ConfigScrollSSD1306 scroll_data = {
//         .time_frame_interval = _25_FRAMES,
//         .vertical_scrolling_offset = 1};
//     display->horizontal_scroll(true, scroll_data);
//     sleep_ms(3000);
//     display->horizontal_scroll(false, scroll_data);
//     sleep_ms(1000);
//     display->vertical_scroll(true, scroll_data);
//     sleep_ms(5000);
//     display->vertical_scroll(false, scroll_data);
//     p1.lo();
// };

void test_write_GDDRAM(void *display_device)
{
    p1.hi();
    struct_SSD1306DataToShow data_to_show;
    struct_RenderArea area;
    static uint8_t image[SSD1306_BUF_LEN];
    uint8_t pattern = 0xF0;

    ((rtos_SSD1306 *)display_device)->clear_device_screen_buffer();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY/2));

    ((rtos_SSD1306 *)display_device)->fill_GDDRAM_with_pattern_and_show(pattern, area);
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY/2));

    pattern = 0xFF;
    area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    memset(image, pattern, area.buflen);
    data_to_show.display = (rtos_SSD1306 *)display_device;
    data_to_show.display_area = area;
    data_to_show.data_buffer = image;
    xQueueSend(display_data_queue, &data_to_show, portMAX_DELAY);
    xSemaphoreTake(data_sent, portMAX_DELAY);

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
        ((rtos_SSD1306 *)received_data_to_show.display)->show_render_area(received_data_to_show.data_buffer, received_data_to_show.display_area);
        xSemaphoreGive(data_sent);
        p4.lo();
    }
}

void main_task(void *display_device)
{
    while (true)
    {
        test_write_GDDRAM((rtos_SSD1306 *)display_device);
        // test_blink((rtos_SSD1306 *)display_device);
        // test_contrast((rtos_SSD1306 *)display_device);
        // test_addressing_mode((rtos_SSD1306 *)display_device);
        // test_scrolling((rtos_SSD1306 *)display_device);
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
