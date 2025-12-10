/**
 * @file test_textual_widget.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @note it seems there compatibility issue between FreeRTOS et std::ostringstream. This test is not performed here.
 * @version 0.1
 * @date 2025-01-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <pico/stdio.h>
#include <math.h>
#include <numbers>
#include <sstream>
#include <iomanip>
#include <string>

#include "sw/widget/rtos_widget.h"
#include "device/ST7735/st7735.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);

#define REFRESH_PERIOD 50
#define DELAY_ms 500
#define LONG_DELAY_ms 1000
#define INTER_TEST_DELAY 1000

#define CANVAS_FORMAT CanvasFormat::RGB565_16b
// #define CANVAS_FORMAT CanvasFormat::MONO_HMSB

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
//==========================display gatekeeper===============
rtos_GraphicDisplayGateKeeper display_gate_keeper = rtos_GraphicDisplayGateKeeper();
//==========================Master SPI========================
struct_ConfigMasterSPI cfg_spi = {
    .spi = spi1,
    .sck_pin = 10,
    .tx_pin = 11,
    .rx_pin = 12,
    .cs_pin = 13,
    .baud_rate_Hz = 10 * 1000 * 1000};
void end_of_TX_DMA_xfer_handler();
rtos_HW_SPI_Master spi_master = rtos_HW_SPI_Master(cfg_spi,
                                                   DMA_IRQ_0, end_of_TX_DMA_xfer_handler);
void end_of_TX_DMA_xfer_handler()
{
    p3.hi();
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    spi_master.spi_tx_dma_isr();
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    p3.lo();
}
//================== ST7735===================
struct_ConfigST7735 cfg_st7735{
    .display_type = DEVICE_DISPLAY_TYPE,
    .backlight_pin = 5,
    .hw_reset_pin = 15,
    .dc_pin = 14,
    .rotation = DEVICE_DISPLAY_ROTATION};
rtos_ST7735 display = rtos_ST7735(&spi_master, cfg_st7735);
//=========================dummy rtos_GraphicWidget for the whole screen===================
class my_text_widget : public rtos_TextWidget
{
private:
public:
    my_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg,
                   CanvasFormat format)
        : rtos_TextWidget(nullptr, text_cfg, format, graphic_display_screen) {};
    my_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg,
                   CanvasFormat format,
                   uint8_t x, uint8_t y)
        : rtos_TextWidget(nullptr, text_cfg, format, x, y, graphic_display_screen) {};
    ~my_text_widget() {};
    void get_value_of_interest() {};
    void draw() {};
};
//===============================TASKS===================================================
void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void display_gate_keeper_task(void *param)
{

    struct_WidgetDataToGateKeeper received_data_to_show;

    while (true)
    {
        xQueueReceive(display_gate_keeper.graphic_widget_data, &received_data_to_show, portMAX_DELAY);
        p4.hi();
        display_gate_keeper.receive_widget_data(received_data_to_show);
        p4.lo();
    }
}

void test_font_size(rtos_ST7735 *current_display)
{
    p1.hi();
    const unsigned char *current_font[4]{font_5x8, font_8x8, font_12x16, font_16x32};
    std::string test_string = "Test";
    struct_ConfigTextWidget default_text_cfg{
        .number_of_column = (uint8_t)test_string.size(),
        .number_of_line = 1,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .font = current_font[0],
        .fg_color = ColorIndex::RED,
        .bg_color = ColorIndex::LIME};

    p2.hi();
    display_gate_keeper.send_clear_device_command(current_display);
    p2.lo();

    p2.hi();
    my_text_widget *font_text_on_screen_0 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    // draw text directly from a string to the pixel buffer
    font_text_on_screen_0->writer->write(test_string.c_str());
    display_gate_keeper.send_widget_data(font_text_on_screen_0);
    p2.lo();

    delete font_text_on_screen_0;
    p2.hi();
    default_text_cfg.widget_anchor_x = 64;
    default_text_cfg.widget_anchor_y = 8;
    my_text_widget *font_text_on_screen_1 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    font_text_on_screen_1->writer->update_canvas_buffer_size(current_font[1]);
    // font_text_on_screen_1->update_widget_anchor(64, 32);
    // process first text according to sprintf capabilities then copy to text buffer and finally draw text buffer into pixel buffer
    sprintf(font_text_on_screen_1->writer->text_buffer, test_string.c_str());
    font_text_on_screen_1->writer->write();
    display_gate_keeper.send_widget_data(font_text_on_screen_1);
    p2.lo();

    delete font_text_on_screen_1;
    p2.hi();
    default_text_cfg.widget_anchor_x = 0;
    default_text_cfg.widget_anchor_y = 16;
    my_text_widget *font_text_on_screen_2 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    font_text_on_screen_2->writer->update_canvas_buffer_size(current_font[2]);
    sprintf(font_text_on_screen_2->writer->text_buffer, test_string.c_str());
    font_text_on_screen_2->writer->write();
    display_gate_keeper.send_widget_data(font_text_on_screen_2);
    p2.lo();

    p2.hi();
    font_text_on_screen_2->writer->update_canvas_buffer_size(current_font[3]);
    font_text_on_screen_2->update_widget_anchor(64, 32);
    sprintf(font_text_on_screen_2->writer->text_buffer, test_string.c_str());
    font_text_on_screen_2->writer->write();
    display_gate_keeper.send_widget_data(font_text_on_screen_2);

    p2.lo();
    delete font_text_on_screen_2;
    p1.lo();

    vTaskDelay(pdMS_TO_TICKS(INTER_TEST_DELAY));
}

void test_full_screen_text(rtos_ST7735 *current_display)
{
    p1.hi();
    struct_ConfigTextWidget txt_conf = {
        .font = font_8x8,
        .fg_color = ColorIndex::CYAN,
        .bg_color = ColorIndex::BLACK,
        .wrap = true,
    };
    my_text_widget text_full_screen = my_text_widget(current_display, txt_conf, CANVAS_FORMAT,
                                                     current_display->TFT_panel_width_in_pixel, current_display->TFT_panel_height_in_pixel);
    p2.hi();
    display_gate_keeper.send_clear_device_command(current_display);
    p2.lo();
    p2.hi();
    text_full_screen.writer->process_char(FORM_FEED);
    display_gate_keeper.send_widget_data(&text_full_screen);
    p2.lo();

    p2.hi();
    for (uint16_t c = 32; c < 256; c++)
    {
        text_full_screen.writer->process_char(c);
        p5.hi();
        display_gate_keeper.send_widget_data(&text_full_screen);
        p5.lo();
    }
    p2.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TEST_DELAY));
}

void test_auto_next_char(rtos_ST7735 *current_display)
{
    p1.hi();
    struct_ConfigTextWidget txt_conf = {
        .font = font_8x8,
        .fg_color = ColorIndex::GOLD,
        .wrap = true,
        .auto_next_char = false};

    my_text_widget *text_frame = new my_text_widget(current_display, txt_conf, CANVAS_FORMAT,
                                                    current_display->TFT_panel_width_in_pixel, current_display->TFT_panel_width_in_pixel);

    text_frame->writer->process_char(FORM_FEED);

    uint16_t n{0};
    for (uint16_t c = 32; c < 128; c++)
    {
        n++;
        text_frame->writer->process_char(c);
        p2.hi();
        display_gate_keeper.send_widget_data(text_frame);
        p2.lo();
        if (n % 8 == 0)
            text_frame->writer->next_char();
    }

    delete text_frame;
    p2.hi();
    display_gate_keeper.send_clear_device_command(current_display);
    p2.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TEST_DELAY));
}

void test_sprintf_format(rtos_ST7735 *current_display)
{
    p1.hi();
    p5.hi();
    display_gate_keeper.send_clear_device_command(current_display);
    p5.lo();

    struct_ConfigTextWidget text_frame_cfg = {
        .font = font_8x8,
        .fg_color = ColorIndex::YELLOW,
        .wrap = true};

    my_text_widget *text_frame = new my_text_widget(current_display, text_frame_cfg, CANVAS_FORMAT,
                                                    current_display->TFT_panel_width_in_pixel, current_display->TFT_panel_width_in_pixel);

    const char *s = "Hello";

    p2.hi();

    text_frame->writer->write("Strings:\n\tpadding:\n");
    display_gate_keeper.send_widget_data(text_frame);

    sprintf(text_frame->writer->text_buffer, "\t[%7s]\n", s);
    text_frame->writer->write();
    display_gate_keeper.send_widget_data(text_frame);

    sprintf(text_frame->writer->text_buffer, "\t[%-7s]\n", s);
    text_frame->writer->write();
    display_gate_keeper.send_widget_data(text_frame);

    sprintf(text_frame->writer->text_buffer, "\t[%*s]\n", 7, s);
    text_frame->writer->write();
    display_gate_keeper.send_widget_data(text_frame);

    text_frame->writer->write("\ttruncating:\n");
    display_gate_keeper.send_widget_data(text_frame);

    sprintf(text_frame->writer->text_buffer, "\t%.4s\n", s);
    text_frame->writer->write();
    display_gate_keeper.send_widget_data(text_frame);

    sprintf(text_frame->writer->text_buffer, "\t\t%.*s\n", 3, s);
    text_frame->writer->write();
    display_gate_keeper.send_widget_data(text_frame);

    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));
    p2.hi();
    p5.hi();
    display_gate_keeper.send_clear_device_command(current_display);
    p5.lo();
    text_frame->writer->clear_text_buffer();
    sprintf(text_frame->writer->text_buffer, "Characters: %c %%", 'A');
    text_frame->writer->write();
    display_gate_keeper.send_widget_data(text_frame);

    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));

    p5.hi();
    display_gate_keeper.send_clear_device_command(current_display);
    p5.lo();

    p2.hi();
    text_frame->writer->update_text_line_column_number(font_5x8);

    text_frame->writer->write("Integers:\n");
    sprintf(text_frame->writer->text_buffer, "\tDec:  %i %d %.3i %i %.0i %+i %i\n", 1, 2, 3, 0, 0, 4, -4);
    text_frame->writer->write();
    sprintf(text_frame->writer->text_buffer, "\tHex:  %x %x %X %#x\n", 5, 10, 10, 6);
    text_frame->writer->write();
    sprintf(text_frame->writer->text_buffer, "\tOct:    %o %#o %#o\n", 10, 10, 4);
    text_frame->writer->write();
    text_frame->writer->write("Floating point:\n");
    sprintf(text_frame->writer->text_buffer, "\tRnd:  %f %.0f %.3f\n", 1.5, 1.5, 1.5);
    text_frame->writer->write();
    sprintf(text_frame->writer->text_buffer, "\tPad:  %05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
    text_frame->writer->write();
    sprintf(text_frame->writer->text_buffer, "\tSci:  %.3E %.1e\n", 1.5, 1.5);
    text_frame->writer->write();
    display_gate_keeper.send_widget_data(text_frame);
    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));
    p5.hi();
    display_gate_keeper.send_clear_device_command(current_display);
    p5.lo();
    p2.hi();
    text_frame->writer->update_text_line_column_number(font_8x8);

    text_frame->writer->process_char(FORM_FEED);

    text_frame->writer->write(" !\"#$%&'()*+,-./0123456789:;<=>?");   // ca 1000us -> 2000us
    text_frame->writer->write("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");   // ca 1000us -> 2000us
    text_frame->writer->write("`abcdefghijklmnopqrstuvwxyz{|}~\x7F"); // ca 1000us-> 2000us
    text_frame->writer->write("1234567890\n");                        // ca 400us -> 800us
    display_gate_keeper.send_widget_data(text_frame);
    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));
    p2.hi();
    text_frame->writer->process_char(FORM_FEED);

    text_frame->writer->write("\t1TAB\n"); // ca 400us -> 800us
    display_gate_keeper.send_widget_data(text_frame);

    vTaskDelay(pdMS_TO_TICKS(DELAY_ms));

    text_frame->writer->write("\t\t2TAB\n"); // ca 400us -> 800us
    display_gate_keeper.send_widget_data(text_frame);

    vTaskDelay(pdMS_TO_TICKS(DELAY_ms));

    text_frame->writer->write("\t\t\t3TAB\n"); // ca 400us -> 800us
    display_gate_keeper.send_widget_data(text_frame);

    vTaskDelay(pdMS_TO_TICKS(DELAY_ms));

    text_frame->writer->write("\t\t\t\t4TAB\n"); // ca 400us -> 800us
    display_gate_keeper.send_widget_data(text_frame);

    vTaskDelay(pdMS_TO_TICKS(DELAY_ms));

    text_frame->writer->write("\t\t\t\t\t5TAB\n"); // ca 400us -> 800us
    display_gate_keeper.send_widget_data(text_frame);

    vTaskDelay(pdMS_TO_TICKS(DELAY_ms));

    text_frame->writer->write("\t1TAB\t\t\t3TAB\n"); // ca 400us -> 800us
    display_gate_keeper.send_widget_data(text_frame);
    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(DELAY_ms));
    p2.hi();
    text_frame->writer->process_char(FORM_FEED);
    text_frame->writer->update_text_line_column_number(font_16x32);

    text_frame->writer->write(" 15:06 \n");
    text_frame->writer->write("03/01/24");
    display_gate_keeper.send_widget_data(text_frame);
    p2.lo();
    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));
    p5.hi();
    display_gate_keeper.send_clear_device_command(current_display);
    p5.lo();
    delete text_frame;
    p2.hi();
    struct_ConfigTextWidget text_frame1_cfg = {
        .number_of_column = 14,
        .number_of_line = 1,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .font = font_5x8,
        .fg_color = ColorIndex::WHITE,
        .bg_color = ColorIndex::BLACK,
        .wrap = false};
    my_text_widget *text_frame1 = new my_text_widget(current_display, text_frame1_cfg, CANVAS_FORMAT);
    text_frame1->writer->write(" 09:56 03JAN24");
    display_gate_keeper.send_widget_data(text_frame);
    delete text_frame1;
    p2.lo();
    p2.hi();
    struct_ConfigTextWidget text_frame2_cfg = {
        .number_of_column = 7,
        .number_of_line = 2,
        .widget_anchor_x = 35,
        .widget_anchor_y = 16,
        .font = font_8x8,
        .fg_color = ColorIndex::YELLOW,
        .bg_color = ColorIndex::BLUE,
        .wrap = false};
    my_text_widget *text_frame2 = new my_text_widget(current_display, text_frame2_cfg, CANVAS_FORMAT);
    text_frame2->writer->write(" 09:56\n03JAN24");
    display_gate_keeper.send_widget_data(text_frame2);
    delete text_frame2;
    p2.lo();
    p2.hi();
    struct_ConfigTextWidget text_frame3_cfg = {
        .number_of_column = 7,
        .number_of_line = 2,
        .widget_anchor_x = 0,
        .widget_anchor_y = 32,
        .font = font_5x8,
        .fg_color = ColorIndex::RED,
        .bg_color = ColorIndex::YELLOW,
        .wrap = false};
    my_text_widget *text_frame3 = new my_text_widget(current_display, text_frame3_cfg, CANVAS_FORMAT);
    text_frame3->writer->write(" 09:56\n03JAN24");
    display_gate_keeper.send_widget_data(text_frame3);
    delete text_frame3;
    p2.lo();
    p2.hi();
    struct_ConfigTextWidget text_frame4_cfg = {
        .number_of_column = 7,
        .number_of_line = 2,
        .widget_anchor_x = 0,
        .widget_anchor_y = 32,
        .font = font_12x16,
        .fg_color = ColorIndex::WHITE,
        .bg_color = ColorIndex::BLACK,
        .wrap = false};
    my_text_widget *text_frame4 = new my_text_widget(current_display, text_frame4_cfg, CANVAS_FORMAT);
    text_frame4->writer->write(" 09:56\n03JAN24");
    display_gate_keeper.send_widget_data(text_frame4);
    delete text_frame4;
    p2.lo();
    p2.hi();
    struct_ConfigTextWidget text_frame5_cfg = {
        .number_of_column = 7,
        .number_of_line = 2,
        .widget_anchor_x = 8,
        .widget_anchor_y = 64,
        .font = font_16x32,
        .fg_color = ColorIndex::YELLOW,
        .bg_color = ColorIndex::BURGUNDY,
        .wrap = false};
    my_text_widget *text_frame5 = new my_text_widget(current_display, text_frame5_cfg, CANVAS_FORMAT);
    text_frame5->writer->write(" 09:56\n03JAN24");
    display_gate_keeper.send_widget_data(text_frame5);
    delete text_frame5;
    p2.lo();
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TEST_DELAY));
    /*
    undefined result for the used compiler
    printf("\tHexadecimal:\t%a %A\n", 1.5, 1.5);
    printf("\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0 / 0.0, 1.0 / 0.0);
    printf("Fixed-width types:\n");
    printf("\tLargest 32-bit value is %" PRIu32 " or %#" PRIx32 "\n",
           UINT32_MAX, UINT32_MAX);
    */
}


void test_monochrome_canvas(rtos_ST7735 *current_display)
{
    p1.hi();
    p2.hi();
    display_gate_keeper.send_clear_device_command(current_display);
    p2.lo();

    std::string test_string = "\xB0\xB3\xB3\xB3\xB3";

    struct_ConfigTextWidget text_cfg{
        .number_of_column = (uint8_t)test_string.size(),
        .number_of_line = 1,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .font = font_5x8,
        .fg_color = ColorIndex::YELLOW,
        .bg_color = ColorIndex::RED};
    p2.hi();
    my_text_widget *mono_text = new my_text_widget(current_display, text_cfg, CANVAS_FORMAT);
    mono_text->writer->write(test_string.c_str());
    display_gate_keeper.send_widget_data(mono_text);
    p2.lo();

    delete mono_text;

    vTaskDelay(pdMS_TO_TICKS(INTER_TEST_DELAY));

    struct_ConfigTextWidget txt_conf2 = {
        .number_of_column = (uint8_t)test_string.size(),
        .number_of_line = 1,
        .widget_anchor_x = 0,
        .widget_anchor_y = 16,
        .font = font_5x8,
        .fg_color = ColorIndex::BLACK,
        .bg_color = ColorIndex::WHITE,
        .wrap = true,
    };

    my_text_widget text_frame = my_text_widget(current_display, txt_conf2, CANVAS_FORMAT);

    text_frame.writer->process_char(FORM_FEED);

    for (auto &&c : test_string)
    {
        text_frame.writer->process_char(c);
        display_gate_keeper.send_widget_data(&text_frame);
    }
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TEST_DELAY));
}

void main_task(void *display_device)
{
    while (true)
    {
        test_monochrome_canvas((rtos_ST7735 *)display_device); //
        test_font_size((rtos_ST7735 *)display_device);         //
        test_full_screen_text((rtos_ST7735 *)display_device);  //
        test_auto_next_char((rtos_ST7735 *)display_device);    //
        test_sprintf_format((rtos_ST7735 *)display_device);    //
    }
}

int main()

{
    p5.hi();

    stdio_init_all();

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(main_task, "main_task", 256, (void *)&display, 2, NULL);
    xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 256, NULL, 4, NULL);
    p5.lo();
    vTaskStartScheduler();

    while (true)
        tight_loop_contents;
}
