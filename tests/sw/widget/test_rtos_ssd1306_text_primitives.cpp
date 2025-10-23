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

#include "sw/widget/widget.h"
#include "device/SSD1306/ssd1306.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);
// Probe p5 = Probe(5);
// Probe p6 = Probe(6);
// Probe p7 = Probe(7);

#define DELAY 10
#define LONG_DELAY 100
#define INTER_TASK_DELAY 100

#define CANVAS_FORMAT CanvasFormat::MONO_VLSB

class my_text_widget : public TextWidget
{
private:
public:
    my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg,
                   CanvasFormat format);
    my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg,
                   CanvasFormat format,
                   uint8_t x, uint8_t y);
    ~my_text_widget();
    void get_value_of_interest();
};
my_text_widget::my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg,
                               CanvasFormat format)
    : TextWidget(graphic_display_screen, text_cfg, format) {}
my_text_widget::my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg,
                               CanvasFormat format,
                               uint8_t x, uint8_t y)
    : TextWidget(graphic_display_screen, text_cfg, format, x, y) {}
my_text_widget::~my_text_widget() {}
void my_text_widget::get_value_of_interest() {}

QueueHandle_t display_data_queue = xQueueCreate(8, sizeof(struct_DataToShow));
SemaphoreHandle_t data_sent = xSemaphoreCreateBinary(); // synchro between display task and sending task

void i2c_irq_handler();

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

rtos_HW_I2C_Master master = rtos_HW_I2C_Master(cfg_i2c);
rtos_SSD1306 left_display = rtos_SSD1306(&master, cfg_left_screen);
rtos_SSD1306 right_display = rtos_SSD1306(&master, cfg_right_screen);

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

void display_gate_keeper_task(void *param)
{
    struct_DataToShow received_data_to_show;

    while (true)
    {
        xQueueReceive(display_data_queue, &received_data_to_show, portMAX_DELAY);
        p4.hi();
        switch (received_data_to_show.command)
        {
        case DisplayCommand::SHOW_IMAGE:
            ((rtos_SSD1306 *)received_data_to_show.display)->show_from_display_queue(received_data_to_show);
            break;
        case DisplayCommand::CLEAR_SCREEN:
            ((rtos_SSD1306 *)received_data_to_show.display)->clear_device_screen_buffer();
            break;
        default:
            break;
        }
        xSemaphoreGive(data_sent);
        p4.lo();
    }
}

void test_font_size(rtos_SSD1306 *current_display)
{
    p1.hi();
    current_display->send_clear_device_command(display_data_queue, data_sent);
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
    font_text_on_screen_0->write(test_string.c_str());
    font_text_on_screen_0->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    delete font_text_on_screen_0;

    default_text_cfg.widget_anchor_x = 64;
    default_text_cfg.widget_anchor_y = 8;
    my_text_widget *font_text_on_screen_1 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    font_text_on_screen_1->update_canvas_buffer_size(current_font[1]);

    // process first text according to sprintf capabilities then copy to text buffer and finally draw text buffer into pixel buffer
    sprintf(font_text_on_screen_1->text_buffer, test_string.c_str());
    font_text_on_screen_1->write();
    font_text_on_screen_1->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    delete font_text_on_screen_1;

    default_text_cfg.widget_anchor_x = 0;
    default_text_cfg.widget_anchor_y = 16;
    my_text_widget *font_text_on_screen_2 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    font_text_on_screen_2->update_canvas_buffer_size(current_font[2]);

    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->write();
    font_text_on_screen_2->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    font_text_on_screen_2->update_canvas_buffer_size(current_font[3]);
    font_text_on_screen_2->update_widget_anchor(64, 32);
    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->write();
    font_text_on_screen_2->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    delete font_text_on_screen_2;
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
    current_display->send_clear_device_command(display_data_queue, data_sent);
}

void test_full_screen_text(rtos_SSD1306 *current_display)
{
    struct_ConfigTextWidget txt_conf = {
        .font = font_8x8,
        .wrap = true,
    };
    p1.hi();
    current_display->send_clear_device_command(display_data_queue, data_sent);
    my_text_widget text_frame = my_text_widget(current_display, txt_conf, CANVAS_FORMAT, SSD1306_WIDTH, SSD1306_HEIGHT);

    text_frame.process_char(FORM_FEED); // equiv. clear full screen
    // current_display->show(text_frame.canvas, 0, 0);
    text_frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    uint16_t nb = text_frame.number_of_line * text_frame.number_of_column;

    uint16_t n{0};
    for (uint16_t c = 32; c < 256; c++)
    {
        n++;
        text_frame.process_char(c);
        // current_display->show(text_frame.canvas, 0, 0);
        text_frame.send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

        if (n == nb)
        {
            sleep_ms(500);
            text_frame.process_char(FORM_FEED);
        }
    }
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
    current_display->send_clear_device_command(display_data_queue, data_sent);
}

void test_auto_next_char(rtos_SSD1306 *current_display)
{
    struct_ConfigTextWidget txt_conf = {
        .font = font_8x8,
        .wrap = true,
        .auto_next_char = false};
    p1.hi();
    current_display->send_clear_device_command(display_data_queue, data_sent);

    my_text_widget *text_frame = new my_text_widget(current_display, txt_conf, CANVAS_FORMAT, SSD1306_WIDTH, SSD1306_HEIGHT);

    text_frame->process_char(FORM_FEED);

    uint16_t n{0};
    for (uint16_t c = 32; c < 128; c++)
    {
        n++;
        text_frame->process_char(c);
        // current_display->show(text_frame->canvas, 0, 0);
        text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

        if (n % 8 == 0)
            text_frame->next_char();
    }

    delete text_frame;
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
    current_display->send_clear_device_command(display_data_queue, data_sent);
}

/**
 * @brief usage of sprintf when we need to format text (e.g. string, float, dec, oct ...)
 *
 * Otherwise text_frame.print_text is sufficient.
 *
 * @param current_display
 */
void test_sprintf_format(rtos_SSD1306 *current_display)
{
    p1.hi();
    current_display->send_clear_device_command(display_data_queue, data_sent);
    struct_ConfigTextWidget text_frame_cfg = {
        .font = font_8x8,
        .wrap = true};

    my_text_widget *text_frame = new my_text_widget(current_display, text_frame_cfg, CANVAS_FORMAT, SSD1306_WIDTH, SSD1306_HEIGHT);

    const char *s = "Hello";

    text_frame->write("Strings:\n\tpadding:\n");
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    sprintf(text_frame->text_buffer, "\t[%7s]\n", s);
    text_frame->write();
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    sprintf(text_frame->text_buffer, "\t[%-7s]\n", s);
    text_frame->write();
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    sprintf(text_frame->text_buffer, "\t[%*s]\n", 7, s);
    text_frame->write();
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    text_frame->write("\ttruncating:\n");
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    sprintf(text_frame->text_buffer, "\t%.4s\n", s);
    text_frame->write();
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    sprintf(text_frame->text_buffer, "\t\t%.*s\n", 3, s);
    text_frame->write();
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));

    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    text_frame->clear_text_buffer();
    // current_display->clear_pixel_buffer(&text_frame->pixel_frame);
    current_display->send_clear_device_command(display_data_queue, data_sent);
    sprintf(text_frame->text_buffer, "Characters: %c %%", 'A');
    text_frame->write();
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));

    current_display->send_clear_device_command(display_data_queue, data_sent);

    text_frame->update_text_frame_size(font_5x8);

    text_frame->write("Integers:\n");
    sprintf(text_frame->text_buffer, "\tDec:  %i %d %.3i %i %.0i %+i %i\n", 1, 2, 3, 0, 0, 4, -4);
    text_frame->write();
    sprintf(text_frame->text_buffer, "\tHex:  %x %x %X %#x\n", 5, 10, 10, 6);
    text_frame->write();
    sprintf(text_frame->text_buffer, "\tOct:    %o %#o %#o\n", 10, 10, 4);
    text_frame->write();
    text_frame->write("Floating point:\n");
    sprintf(text_frame->text_buffer, "\tRnd:  %f %.0f %.3f\n", 1.5, 1.5, 1.5);
    text_frame->write();
    sprintf(text_frame->text_buffer, "\tPad:  %05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
    text_frame->write();
    sprintf(text_frame->text_buffer, "\tSci:  %.3E %.1e\n", 1.5, 1.5);
    text_frame->write();
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));

    current_display->send_clear_device_command(display_data_queue, data_sent);

    text_frame->update_text_frame_size(font_8x8);

    text_frame->process_char(FORM_FEED); // equivalent text_frame->clear_pixel_buffer();

    text_frame->write(" !\"#$%&'()*+,-./0123456789:;<=>?");   // ca 1000us -> 2000us
    text_frame->write("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");   // ca 1000us -> 2000us
    text_frame->write("`abcdefghijklmnopqrstuvwxyz{|}~\x7F"); // ca 1000us-> 2000us
    text_frame->write("1234567890\n");                        // ca 400us -> 800us
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));

    text_frame->process_char(FORM_FEED);

    text_frame->write("\t1TAB\n"); // ca 400us -> 800us
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(DELAY));

    text_frame->write("\t\t2TAB\n"); // ca 400us -> 800us
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(DELAY));

    text_frame->write("\t\t\t3TAB\n"); // ca 400us -> 800us
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(DELAY));

    text_frame->write("\t\t\t\t4TAB\n"); // ca 400us -> 800us
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(DELAY));

    text_frame->write("\t\t\t\t\t5TAB\n"); // ca 400us -> 800us
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(DELAY));

    text_frame->write("\t1TAB\t\t\t3TAB\n"); // ca 400us -> 800us
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));

    text_frame->process_char(FORM_FEED);
    text_frame->update_text_frame_size(font_16x32);

    text_frame->write(" 15:06 \n");
    text_frame->write("03/01/24");
    text_frame->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);

    vTaskDelay(pdMS_TO_TICKS(LONG_DELAY));

    current_display->send_clear_device_command(display_data_queue, data_sent);

    delete text_frame;

    struct_ConfigTextWidget text_frame2_cfg = {
        .number_of_column = 7,
        .number_of_line = 2,
        .widget_anchor_x = 22,
        .widget_anchor_y = 16,
        .font = font_12x16,
        .wrap = false};
    my_text_widget *text_frame2 = new my_text_widget(current_display, text_frame2_cfg, CANVAS_FORMAT);

    text_frame2->write(" 09:56\n03JAN24");
    text_frame2->send_image_to_DisplayGateKeeper(display_data_queue, data_sent);
    delete text_frame2;
    p1.lo();
    vTaskDelay(pdMS_TO_TICKS(INTER_TASK_DELAY));
    current_display->send_clear_device_command(display_data_queue, data_sent);

    /*
    undefined result for the used compiler
    printf("\tHexadecimal:\t%a %A\n", 1.5, 1.5);
    printf("\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0 / 0.0, 1.0 / 0.0);
    printf("Fixed-width types:\n");
    printf("\tLargest 32-bit value is %" PRIu32 " or %#" PRIx32 "\n",
           UINT32_MAX, UINT32_MAX);
    */
}

void main_task(void *display_device)
{
    while (true)
    {
        test_font_size((rtos_SSD1306 *)display_device);
        test_full_screen_text((rtos_SSD1306 *)display_device);
        test_auto_next_char((rtos_SSD1306 *)display_device);
        test_sprintf_format((rtos_SSD1306 *)display_device);
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
