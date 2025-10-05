/**
 * @file test_st7735_graphic_primitives.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#include <numbers>
#include <sstream>
#include <iomanip>

#include "device/ST7735/st7735.h"
#include "sw/widget/widget.h"

#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
// Probe p6 = Probe(6);
// Probe p7 = Probe(7);

#define CANVAS_FORMAT CanvasFormat::trueRGB565

#define INTRA_TASK_DELAY 500
#define INTER_TASK_DELAY 1000

// #define ST7735_128x128
#define ST7735_128x160

#ifdef ST7735_128x128
#define DEVICE_DISPLAY_TYPE ST7735DisplayType::ST7735_144_128_RGB_128_GREENTAB
#define DEVICE_DISPLAY_ROTATION ST7735Rotation::_90
#define DEVICE_DISPLAY_HEIGHT 128
#endif
#ifdef ST7735_128x160
#define DEVICE_DISPLAY_TYPE ST7735DisplayType::ST7735_177_160_RGB_128_GREENTAB
#define DEVICE_DISPLAY_ROTATION ST7735Rotation::_180
#define DEVICE_DISPLAY_HEIGHT 160
#endif

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
    .rotation = DEVICE_DISPLAY_ROTATION,
};

struct_ConfigGraphicWidget full_screen_cfg = {
    .pixel_frame_width = 128,
    .pixel_frame_height = DEVICE_DISPLAY_HEIGHT,
    .fg_color = ColorIndex::RED,
    .bg_color = ColorIndex::YELLOW,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};

QueueHandle_t data_to_show_queue = xQueueCreate(8, sizeof(struct_ST7735_data_to_show));
SemaphoreHandle_t data_sent = xSemaphoreCreateBinary();

class my_full_screen_widget : public GraphicWidget
{
private:
public:
    my_full_screen_widget(GraphicDisplayDevice *graphic_display_screen,
                          struct_ConfigGraphicWidget graph_cfg, CanvasFormat format);
    ~my_full_screen_widget();
    void get_value_of_interest();
    void draw();
};
my_full_screen_widget::my_full_screen_widget(GraphicDisplayDevice *graphic_display_screen,
                                             struct_ConfigGraphicWidget graph_cfg, CanvasFormat format)
    : GraphicWidget(graphic_display_screen, graph_cfg, format) {}
my_full_screen_widget::~my_full_screen_widget() {}
void my_full_screen_widget::get_value_of_interest() {}
void my_full_screen_widget::draw() {};

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void end_of_TX_DMA_xfer_handler();
void end_of_RX_DMA_xfer_handler();

rtos_HW_SPI_Master spi_master = rtos_HW_SPI_Master(cfg_spi,
                                                   DMA_IRQ_1, end_of_TX_DMA_xfer_handler,
                                                   DMA_IRQ_0, end_of_RX_DMA_xfer_handler);

void display_show_task(void *param)
{
    struct_ST7735_data_to_show received_data_to_show;
    size_t canvas_buffer_size = received_data_to_show.canvas->canvas_buffer_size_pixel;
    while (true)
    {
        xQueueReceive(data_to_show_queue, &received_data_to_show, portMAX_DELAY);
        p3.hi();
        received_data_to_show.display->show(received_data_to_show.canvas,
                                            received_data_to_show.anchor_x, received_data_to_show.anchor_y);
        xSemaphoreGive(data_sent);
        p3.lo();
    }
}

void end_of_TX_DMA_xfer_handler()
{
    p4.hi();
    spi_master.spi_tx_dma_isr();
    p4.lo();
}
void end_of_RX_DMA_xfer_handler()
{
    p5.hi();
    spi_master.spi_rx_dma_isr();
    p5.lo();
}

void test_fb_line(ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    display->clear_device_screen_buffer(); //TODO optimize clear_device_screen_buffer() to use DMA (65ms)
    p1.lo();
    p1.hi();
    frame.canvas->clear_canvas_buffer(); //TODO optimize clear_canvas_buffer() to use DMA (165us)
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
    p1.hi();

    struct_ST7735_data_to_show data_to_show; // TODO sequence à porter dans frame ou display
    data_to_show.display = display;
    data_to_show.canvas = frame.canvas; 
    data_to_show.anchor_x = frame.widget_anchor_x;
    data_to_show.anchor_y = frame.widget_anchor_y;
    
    p2.hi();
    int i = 0;
    for (int x = 0; x < 128; x++)
    {
        i++;
        if (i > 20 * 8)
        i = 1;
        frame.line(x, 0, 128 - 1 - x, 128 - 1, static_cast<ColorIndex>(i / 8));
 
        xQueueSend(data_to_show_queue, &data_to_show, portMAX_DELAY);// TODO sequence à porter dans frame ou display
        xSemaphoreTake(data_sent, portMAX_DELAY);

    }
    p2.lo();
    p2.hi();
    
    for (int y = 128 - 1; y >= 0; y--)
    {
        i++;
        if (i > 20 * 8)
        i = 1;
        frame.line(0, y, 128 - 1, 128 - 1 - y, static_cast<ColorIndex>(i / 8));

        xQueueSend(data_to_show_queue, &data_to_show, portMAX_DELAY);// TODO sequence à porter dans frame ou display
        xSemaphoreTake(data_sent, portMAX_DELAY);

    }
    p2.lo();
    p1.lo();
}
void test_outofframe_line(ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);
    int y0, x1, y1;
    display->clear_device_screen_buffer();
    frame.canvas->clear_canvas_buffer();
    x1 = 64;
    y1 = 160;
    y0 = -10;

    uint8_t i = 0;
    p1.lo();
    p2.hi();

    for (int x = -10; x < 138; x++)
    {
        i++;
        if (i > 20 * 8)
            i = 0;
        ColorIndex c = static_cast<ColorIndex>(i / 8);
        frame.line(x, y0, x1, y1, c);
        p3.hi();
        frame.show();
        p3.lo();
    }
    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}
void test_fb_rect(ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);

    display->clear_device_screen_buffer();
    frame.canvas->clear_canvas_buffer();
    frame.rect(0, 0, 128, 64, false, ColorIndex::RED);
    p3.hi();
    frame.show();
    p3.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    p2.hi();
    frame.rect(10, 10, 108, 44, true, ColorIndex::YELLOW);
    p3.hi();
    frame.show();
    p3.lo();
    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}
void test_fb_hline(ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);

    display->clear_device_screen_buffer(); // 50ms @ bitrate 10Mbps
    frame.canvas->clear_canvas_buffer();
    p1.lo();
    p2.hi();
    for (size_t i = 0; i < 16; i++)
    {
        frame.hline(0, i * 8, 128, static_cast<ColorIndex>(i + 1));
        p3.hi();
        frame.show();
        p3.lo();
    }
    p3.hi();
    frame.show(); // 80ms @ bitrate 10Mbps
    p3.lo();
    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}
void test_fb_vline(ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);

    display->clear_device_screen_buffer(); // 50ms @ bitrate 10Mbps
    frame.canvas->clear_canvas_buffer();
    p1.lo();
    p2.hi();
    for (size_t i = 0; i < 16; i++)
    {
        frame.vline(i * 8, 0, 128, static_cast<ColorIndex>(i + 1));
        p3.hi();
        frame.show();
        p3.lo();
    }
    p3.hi();
    frame.show(); // 80ms @ bitrate 10Mbps
    p3.lo();
    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}

void test_fb_circle(ST7735 *display)
{
    p1.hi();
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);

    display->clear_device_screen_buffer();
    frame.canvas->clear_canvas_buffer();
    frame.circle(50, 63, 31, false, ColorIndex::ORANGE);
    p3.hi();
    frame.show();
    p3.lo();
    p1.lo();
    p2.hi();
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));
    frame.circle(20, 64, 32, true, ColorIndex::LIME);
    p3.hi();
    frame.show();
    p3.lo();
    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}

void test_fb_in_fb(ST7735 *display)
{
    my_full_screen_widget frame = my_full_screen_widget(display, full_screen_cfg, CANVAS_FORMAT);

    display->clear_device_screen_buffer();
    frame.canvas->clear_canvas_buffer();
    frame.rect(0, 0, display->TFT_panel_width_in_pixel, display->TFT_panel_height_in_pixel);
    frame.rect(10, 10, 108, 44, true, ColorIndex::CYAN);
    frame.line(5, 60, 120, 5, ColorIndex::RED);
    p3.hi();
    frame.show();
    p3.lo();
    vTaskDelay(pdMS_TO_TICKS(INTRA_TASK_DELAY));

    struct_ConfigGraphicWidget small_frame_cfg{
        .pixel_frame_width = 80,
        .pixel_frame_height = 24,
        .widget_anchor_x = 24,
        .widget_anchor_y = 24};

    my_full_screen_widget small_frame = my_full_screen_widget(display, small_frame_cfg, CANVAS_FORMAT);
    small_frame.canvas->fill_canvas_with_color(ColorIndex::NAVY);
    small_frame.line(5, 5, 80, 20, ColorIndex::YELLOW);
    small_frame.circle(8, 44, 12, false, ColorIndex::GREEN);
    p3.hi();
    small_frame.show();
    p3.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
}

void main_task(void *display_device)
{
    while (true)
    {
        test_fb_line((ST7735 *)display_device);
        // test_outofframe_line((ST7735 *)display_device);
        // test_fb_hline((ST7735 *)display_device);
        // test_fb_vline((ST7735 *)display_device);
        // test_fb_rect((ST7735 *)display_device);
        // test_fb_circle((ST7735 *)display_device);
        // test_fb_in_fb((ST7735 *)display_device);
    }
}

int main()
{
    p5.hi();
    stdio_init_all();
    rtos_ST7735 display = rtos_ST7735(&spi_master, cfg_st7735);

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(main_task, "main_task", 256, (void *)&display, 2, NULL);
    xTaskCreate(display_show_task, "display_show_task", 250, NULL, 4, NULL);

    p5.lo();
    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
