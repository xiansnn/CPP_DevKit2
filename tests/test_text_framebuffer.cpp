/**
 * @file test_textual_widget.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
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

#include "probe.h"
#include "hw_i2c.h"
#include "ssd1306.h"
#include "framebuffer.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

#define REFRESH_PERIOD 50
#define DELAY 500
#define LONG_DELAY 1000
#define INTER_TEST_DELAY 2000

struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};

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

void test_font_size(SSD1306 *current_display)
{
    current_display->clear_device_screen_buffer();
    const unsigned char *current_font[4]{font_5x8, font_8x8, font_12x16, font_16x32};
    uint8_t current_x_anchor = 0;
    uint8_t current_y_anchor = 0;

    std::string test_string = "Test";

    struct_ConfigTextFramebuffer default_text_cfg{
        .number_of_column = (uint8_t)test_string.size(),
        .number_of_line = 1,
        .font = current_font[0]};

    TextualFrameBuffer *font_text_on_screen_0 = new TextualFrameBuffer(current_display, default_text_cfg);
    sprintf(font_text_on_screen_0->text_buffer, test_string.c_str());
    font_text_on_screen_0->print_text_buffer();
    current_display->show(&font_text_on_screen_0->pixel_memory, current_x_anchor, current_y_anchor);
    delete font_text_on_screen_0;

    TextualFrameBuffer *font_text_on_screen_1 = new TextualFrameBuffer(current_display, default_text_cfg);
    font_text_on_screen_1->update_pixel_area(current_font[1]);
    current_x_anchor = 64;
    current_y_anchor = 8;
    sprintf(font_text_on_screen_1->text_buffer, test_string.c_str());
    font_text_on_screen_1->print_text_buffer();
    current_display->show(&font_text_on_screen_1->pixel_memory, current_x_anchor, current_y_anchor);
    delete font_text_on_screen_1;

    TextualFrameBuffer *font_text_on_screen_2 = new TextualFrameBuffer(current_display, default_text_cfg);
    font_text_on_screen_2->update_pixel_area(current_font[2]);
    current_x_anchor = 0;
    current_y_anchor = 16;
    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->print_text_buffer();
    current_display->show(&font_text_on_screen_2->pixel_memory, current_x_anchor, current_y_anchor);

    font_text_on_screen_2->update_pixel_area(current_font[3]);
    current_x_anchor = 64;
    current_y_anchor = 32;
    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->print_text_buffer();
    current_display->show(&font_text_on_screen_2->pixel_memory, current_x_anchor, current_y_anchor);
    delete font_text_on_screen_2;

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_device_screen_buffer();
}

void test_full_screen_text(SSD1306 *current_display)
{
    struct_ConfigTextFramebuffer txt_conf = {
        .font = font_8x8,
        .wrap = true,
    };
    TextualFrameBuffer text_frame = TextualFrameBuffer(current_display, SSD1306_WIDTH, SSD1306_HEIGHT, txt_conf);

    text_frame.print_char(FORM_FEED); // equiv. clear full screen
    current_display->show(&text_frame.pixel_memory, 0, 0);
    uint16_t nb = text_frame.frame_text_config.number_of_line * text_frame.frame_text_config.number_of_column;

    uint16_t n{0};
    for (uint16_t c = 32; c < 256; c++)
    {
        n++;
        text_frame.print_char(c);
        current_display->show(&text_frame.pixel_memory, 0, 0);
        if (n == nb)
        {
            sleep_ms(500);
            text_frame.print_char(FORM_FEED);
        }
    }

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_device_screen_buffer();
}

void test_auto_next_char(SSD1306 *current_display)
{
    struct_ConfigTextFramebuffer txt_conf = {
        .font = font_8x8,
        .wrap = true,
        .auto_next_char = false};

    TextualFrameBuffer *text_frame = new TextualFrameBuffer(current_display, SSD1306_WIDTH, SSD1306_HEIGHT, txt_conf);

    text_frame->print_char(FORM_FEED);

    uint16_t n{0};
    for (uint16_t c = 32; c < 128; c++)
    {
        n++;
        text_frame->print_char(c);
        current_display->show(&text_frame->pixel_memory, 0, 0);
        if (n % 5 == 0)
        {
            text_frame->next_char();
        }
    }

    delete text_frame;

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_device_screen_buffer();
}

/**
 * @brief usage of sprintf when we need to format text (e.g. string, float, dec, oct ...)
 *
 * Otherwise text_frame.print_text is sufficient.
 *
 * @param current_display
 */
void test_sprintf_format(SSD1306 *current_display)
{
    current_display->clear_device_screen_buffer();

    struct_ConfigTextFramebuffer text_frame_cfg = {
        .font = font_8x8,
        .wrap = true};

    TextualFrameBuffer *text_frame = new TextualFrameBuffer(current_display, SSD1306_WIDTH, SSD1306_HEIGHT, text_frame_cfg);

    const char *s = "Hello";

    text_frame->print_text("Strings:\n\tpadding:\n");
    current_display->show(&text_frame->pixel_memory, 0, 0);

    sprintf(text_frame->text_buffer, "\t[%7s]\n", s);
    text_frame->print_text_buffer();
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sprintf(text_frame->text_buffer, "\t[%-7s]\n", s);
    text_frame->print_text_buffer();
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sprintf(text_frame->text_buffer, "\t[%*s]\n", 7, s);
    text_frame->print_text_buffer();
    current_display->show(&text_frame->pixel_memory, 0, 0);
    text_frame->print_text("\ttruncating:\n");
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sprintf(text_frame->text_buffer, "\t%.4s\n", s);
    text_frame->print_text_buffer();
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sprintf(text_frame->text_buffer, "\t\t%.*s\n", 3, s);
    text_frame->print_text_buffer();
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sleep_ms(LONG_DELAY);

    current_display->clear_device_screen_buffer();
    text_frame->clear_text_buffer();
    sprintf(text_frame->text_buffer, "Characters: %c %%", 'A');
    text_frame->print_text_buffer();
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sleep_ms(LONG_DELAY);

    current_display->clear_device_screen_buffer();
    text_frame->update_text_buffer_size(font_5x8);
    text_frame->clear_text_buffer();

    text_frame->print_text("Integers:\n");
    sprintf(text_frame->text_buffer, "\tDec:  %i %d %.3i %i %.0i %+i %i\n", 1, 2, 3, 0, 0, 4, -4);
    text_frame->print_text_buffer();
    sprintf(text_frame->text_buffer, "\tHex:  %x %x %X %#x\n", 5, 10, 10, 6);
    text_frame->print_text_buffer();
    sprintf(text_frame->text_buffer, "\tOct:    %o %#o %#o\n", 10, 10, 4);
    text_frame->print_text_buffer();
    text_frame->print_text("Floating point:\n");
    sprintf(text_frame->text_buffer, "\tRnd:  %f %.0f %.3f\n", 1.5, 1.5, 1.5);
    text_frame->print_text_buffer();
    sprintf(text_frame->text_buffer, "\tPad:  %05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
    text_frame->print_text_buffer();
    sprintf(text_frame->text_buffer, "\tSci:  %.3E %.1e\n", 1.5, 1.5);
    text_frame->print_text_buffer();
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sleep_ms(LONG_DELAY);

    current_display->clear_device_screen_buffer();

    text_frame->update_text_buffer_size(font_8x8);

    text_frame->print_char(FORM_FEED); // equivalent text_frame->clear_pixel_buffer();

    text_frame->print_text(" !\"#$%&'()*+,-./0123456789:;<=>?");   // ca 1000us -> 2000us
    text_frame->print_text("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");   // ca 1000us -> 2000us
    text_frame->print_text("`abcdefghijklmnopqrstuvwxyz{|}~\x7F"); // ca 1000us-> 2000us
    text_frame->print_text("1234567890\n");                        // ca 400us -> 800us
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sleep_ms(LONG_DELAY);

    text_frame->print_char(FORM_FEED);

    text_frame->print_text("\t1TAB\n"); // ca 400us -> 800us
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_text("\t\t2TAB\n"); // ca 400us -> 800us
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_text("\t\t\t3TAB\n"); // ca 400us -> 800us
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_text("\t\t\t\t4TAB\n"); // ca 400us -> 800us
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_text("\t\t\t\t\t5TAB\n"); // ca 400us -> 800us
    current_display->show(&text_frame->pixel_memory, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_text("\t1TAB\t\t\t3TAB\n"); // ca 400us -> 800us
    current_display->show(&text_frame->pixel_memory, 0, 0);

    sleep_ms(LONG_DELAY);
    text_frame->print_char(FORM_FEED);

    text_frame->update_text_buffer_size(font_16x32);
    text_frame->print_text(" 15:06 \n");
    text_frame->print_text("03/01/24");
    current_display->show(&text_frame->pixel_memory, 0, 0);

    sleep_ms(LONG_DELAY);
    current_display->clear_device_screen_buffer();

    delete text_frame;

    struct_ConfigTextFramebuffer text_frame2_cfg = {
        .number_of_column =7,
        .number_of_line = 2,
        .font = font_12x16,
        .wrap = false};
    TextualFrameBuffer *text_frame2 = new TextualFrameBuffer(current_display, text_frame2_cfg);

    text_frame2->print_text(" 09:56\n"); 
    text_frame2->print_text("03JAN24");
    current_display->show(&text_frame2->pixel_memory, 22, 16);
    delete text_frame2;

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_device_screen_buffer();
    /*
    undefined result for the used compiler
    printf("\tHexadecimal:\t%a %A\n", 1.5, 1.5);
    printf("\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0 / 0.0, 1.0 / 0.0);
    printf("Fixed-width types:\n");
    printf("\tLargest 32-bit value is %" PRIu32 " or %#" PRIx32 "\n",
           UINT32_MAX, UINT32_MAX);
    */
}

/**
 * @brief Another way to print formatted text, using stringstream
 *
 * @param current_display
 */
void test_ostringstream_format(SSD1306 *current_display)
{

    current_display->clear_device_screen_buffer();

    const unsigned char *current_font{font_5x8};

    struct_ConfigTextFramebuffer txt_conf = {
        .font = current_font,
        .wrap = false};

    TextualFrameBuffer text_frame = TextualFrameBuffer(current_display, SSD1306_WIDTH, SSD1306_HEIGHT, txt_conf);

    int n = 42;
    float f = std::numbers::pi;

    std::ostringstream stream0, stream1, stream2;

    stream0.fill('.');
    stream2.fill('.');
    stream2.precision(4);
    stream2.width(20);

    stream0 << std::left << std::setw(6) << "test" << std::endl;
    text_frame.print_text(stream0.str().c_str());

    current_display->show(&text_frame.pixel_memory, 0, 0);
    sleep_ms(DELAY);

    stream1 << std::setw(5) << std::dec << n << "|" << std::setw(5)
            << std::showbase << std::hex << n << "|" << std::showbase << std::setw(5) << std::oct << n << std::endl;
    text_frame.print_text(stream1.str().c_str());
    current_display->show(&text_frame.pixel_memory, 0, 0);

    sleep_ms(DELAY);

    stream2 << "PI = " << std::left << f << std::endl;
    text_frame.print_text(stream2.str().c_str());
    current_display->show(&text_frame.pixel_memory, 0, 0);

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_device_screen_buffer();
}

int main()

{
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 left_display = SSD1306(&master, cfg_left_screen);
    SSD1306 right_display = SSD1306(&master, cfg_right_screen);
    left_display.clear_device_screen_buffer();
    right_display.clear_device_screen_buffer();

    while (true)
    {
        test_font_size(&left_display);
        test_full_screen_text(&right_display);
        test_auto_next_char(&left_display);
        test_sprintf_format(&right_display);
        test_ostringstream_format(&left_display);
    }
}
