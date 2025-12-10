/**
 * @file test_rtos_ssd1306_text_primitives.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-10-22
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
#include "device/SSD1306/ssd1306.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
// Probe p2 = Probe(2);
// Probe p3 = Probe(3);
Probe p4 = Probe(4);
// Probe p5 = Probe(5);
// Probe p6 = Probe(6);
// Probe p7 = Probe(7);

#define DELAY_ms 50
#define LONG_DELAY_ms 1000
#define INTER_TASK_DELAY_ms 2000

#define CANVAS_FORMAT CanvasFormat::MONO_VLSB

//==========================display gatekeeper===============
rtos_GraphicDisplayGateKeeper display_gate_keeper = rtos_GraphicDisplayGateKeeper();
//==========================Master I2C========================
void i2c_irq_handler();
struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE,
    .i2c_tx_master_handler = i2c_irq_handler};
rtos_HW_I2C_Master master = rtos_HW_I2C_Master(cfg_i2c);
void i2c_irq_handler()
{
    master.i2c_dma_isr();
};
//==================SSD1306 left screen===================
struct_ConfigSSD1306 cfg_left_screen{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};
rtos_SSD1306 left_display = rtos_SSD1306(&master, cfg_left_screen);
//==================SSD1306 right screen===================
struct_ConfigSSD1306 cfg_right_screen{
    .i2c_address = 0x3D,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};
rtos_SSD1306 right_display = rtos_SSD1306(&master, cfg_right_screen);
///=========================dummy rtos_GraphicWidget for the whole screen===================
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
    UBaseType_t msg_nb;
    struct_WidgetDataToGateKeeper received_data_to_show;
    while (true)
    {
        xQueueReceive(display_gate_keeper.graphic_widget_data, &received_data_to_show, portMAX_DELAY);
        msg_nb = uxQueueMessagesWaiting(display_gate_keeper.graphic_widget_data);
        if (msg_nb != 0)
            p4.pulse_train(msg_nb);

        display_gate_keeper.receive_widget_data(received_data_to_show);
    }
}

void test_font_size(rtos_SSD1306 *current_display)
{
    uint display_id = (current_display == &left_display) ? 0 : 100;
    display_gate_keeper.send_clear_device_command(current_display);
    const unsigned char *current_font[4]{font_5x8, font_8x8, font_12x16, font_16x32};

    std::string test_string = "Test";

    struct_ConfigTextWidget default_text_cfg{
        .number_of_column = (uint8_t)test_string.size(),
        .number_of_line = 1,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .font = current_font[0]};

    my_text_widget *font_text_on_screen_0 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    // draw text directly from a string to the pixel buffer
    font_text_on_screen_0->writer->write(test_string.c_str());
    p1.pulse_train(display_id + 10);
    display_gate_keeper.send_widget_data(font_text_on_screen_0);
    delete font_text_on_screen_0;

    default_text_cfg.widget_anchor_x = 64;
    default_text_cfg.widget_anchor_y = 8;
    default_text_cfg.font = current_font[1];
    my_text_widget *font_text_on_screen_1 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    // process first text according to sprintf capabilities then copy to text buffer and finally draw text buffer into pixel buffer
    sprintf(font_text_on_screen_1->writer->text_buffer, test_string.c_str());
    font_text_on_screen_1->writer->write();
    p1.pulse_train(display_id + 20);
    display_gate_keeper.send_widget_data(font_text_on_screen_1);
    delete font_text_on_screen_1;

    default_text_cfg.widget_anchor_x = 0;
    default_text_cfg.widget_anchor_y = 16;
    default_text_cfg.font = current_font[2];
    my_text_widget *font_text_on_screen_2 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    // font_text_on_screen_2->writer->update_canvas_buffer_size(current_font[2]);

    sprintf(font_text_on_screen_2->writer->text_buffer, test_string.c_str());
    font_text_on_screen_2->writer->write();
    p1.pulse_train(display_id + 30);
    display_gate_keeper.send_widget_data(font_text_on_screen_2);
    delete font_text_on_screen_2;

    my_text_widget *font_text_on_screen_3 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    font_text_on_screen_3->writer->update_canvas_buffer_size(current_font[3]);
    font_text_on_screen_3->update_widget_anchor(64, 32);
    sprintf(font_text_on_screen_3->writer->text_buffer, test_string.c_str());
    font_text_on_screen_3->writer->write();
    p1.pulse_train(display_id + 40);
    display_gate_keeper.send_widget_data(font_text_on_screen_3);
    delete font_text_on_screen_3;

    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}

void test_full_screen_text(rtos_SSD1306 *current_display)
{
    struct_ConfigTextWidget txt_conf = {
        .font = font_8x8,
        .wrap = true,
    };
    p1.hi();
    display_gate_keeper.send_clear_device_command(current_display);
    my_text_widget text_frame = my_text_widget(current_display, txt_conf, CANVAS_FORMAT, SSD1306_WIDTH, SSD1306_HEIGHT);

    text_frame.writer->process_char(FORM_FEED); // equiv. clear full screen
    display_gate_keeper.send_widget_data(&text_frame);
    uint16_t nb = text_frame.writer->number_of_line * text_frame.writer->number_of_column;

    uint16_t n{0};
    for (uint16_t c = 32; c < 256; c++)
    {
        n++;
        text_frame.writer->process_char(c);
        display_gate_keeper.send_widget_data(&text_frame);

        if (n == nb)
        {
            vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));
            text_frame.writer->process_char(FORM_FEED);
        }
    }
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}

void test_auto_next_char(rtos_SSD1306 *current_display)
{
    struct_ConfigTextWidget txt_conf = {
        .font = font_8x8,
        .wrap = true,
        .auto_next_char = false};
    p1.hi();
    display_gate_keeper.send_clear_device_command(current_display);

    my_text_widget *text_frame = new my_text_widget(current_display, txt_conf, CANVAS_FORMAT, SSD1306_WIDTH, SSD1306_HEIGHT);

    text_frame->writer->process_char(FORM_FEED);

    uint16_t n{0};
    for (uint16_t c = 32; c < 128; c++)
    {
        n++;
        text_frame->writer->process_char(c);
        display_gate_keeper.send_widget_data(text_frame);

        if (n % 8 == 0)
            text_frame->writer->next_char();
    }
    delete text_frame;
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}

void test_sprintf_format(rtos_SSD1306 *current_display)
{
    struct_ConfigTextWidget text_frame_cfg = {
        .font = font_8x8,
        .wrap = true};
    p1.hi();
    display_gate_keeper.send_clear_device_command(current_display);

    my_text_widget *text_frame = new my_text_widget(current_display, text_frame_cfg, CANVAS_FORMAT, SSD1306_WIDTH, SSD1306_HEIGHT);

    const char *s = "Hello";

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

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));

    display_gate_keeper.send_widget_data(text_frame);

    text_frame->writer->clear_text_buffer();
    display_gate_keeper.send_clear_device_command(current_display);
    sprintf(text_frame->writer->text_buffer, "Characters: %c %%", 'A');
    text_frame->writer->write();
    display_gate_keeper.send_widget_data(text_frame);

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));

    display_gate_keeper.send_clear_device_command(current_display);

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

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));

    display_gate_keeper.send_clear_device_command(current_display);

    text_frame->writer->update_text_line_column_number(font_8x8);

    text_frame->writer->process_char(FORM_FEED); // equivalent text_frame->clear_pixel_buffer();

    text_frame->writer->write(" !\"#$%&'()*+,-./0123456789:;<=>?");   // ca 1000us -> 2000us
    text_frame->writer->write("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");   // ca 1000us -> 2000us
    text_frame->writer->write("`abcdefghijklmnopqrstuvwxyz{|}~\x7F"); // ca 1000us-> 2000us
    text_frame->writer->write("1234567890\n");                        // ca 400us -> 800us
    display_gate_keeper.send_widget_data(text_frame);

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));

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

    delete text_frame;
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));

    /*
    undefined result for the used compiler
    printf("\tHexadecimal:\t%a %A\n", 1.5, 1.5);
    printf("\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0 / 0.0, 1.0 / 0.0);
    printf("Fixed-width types:\n");
    printf("\tLargest 32-bit value is %" PRIu32 " or %#" PRIx32 "\n",
           UINT32_MAX, UINT32_MAX);
    */
}

void test_sprintf_format_date(rtos_SSD1306 *current_display)
{
    struct_ConfigTextWidget text_frame_cfg = {
        .number_of_column = 8,
        .number_of_line = 2,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .font = font_12x16,
        .wrap = false};
    p1.hi();
    display_gate_keeper.send_clear_device_command(current_display);

    my_text_widget *text_frame = new my_text_widget(current_display, text_frame_cfg, CANVAS_FORMAT);

    text_frame->writer->write(" 15:06 \n");
    text_frame->writer->write("08/12/25");
    display_gate_keeper.send_widget_data(text_frame);

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY_ms));

    text_frame->update_widget_anchor(0, 0);
    text_frame->writer->update_canvas_buffer_size(font_16x32);

    text_frame->writer->process_char(FORM_FEED);

    text_frame->writer->write(" 10:14 \n08DEC25");
    display_gate_keeper.send_widget_data(text_frame);
    delete text_frame;
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY_ms));
}

void main_task(void *display_device)
{
    while (true)
    {
        test_font_size((rtos_SSD1306 *)display_device);
        test_full_screen_text((rtos_SSD1306 *)display_device);
        test_auto_next_char((rtos_SSD1306 *)display_device);
        test_sprintf_format((rtos_SSD1306 *)display_device);
        test_sprintf_format_date((rtos_SSD1306 *)display_device);
    }
}

int main()
{
    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(main_task, "right_main_task", 256, (void *)&right_display, 2, NULL);
    xTaskCreate(main_task, "left_main_task", 256, (void *)&left_display, 2, NULL);
    xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 256, NULL, 4, NULL);

    vTaskStartScheduler();

    while (true)
        tight_loop_contents();
}
