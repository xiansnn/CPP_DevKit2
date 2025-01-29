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
    current_display->clear_full_screen();
    const unsigned char *current_font[4]{font_5x8, font_8x8, font_12x16, font_16x32};
    uint8_t current_x_anchor = 0;
    uint8_t current_y_anchor = 0;

    std::string test_string = "Test";

    TextualFrameBuffer *font_text_on_screen_0 = new TextualFrameBuffer(test_string.size(), 1, {.font = current_font[0]});

    sprintf(font_text_on_screen_0->text_buffer, test_string.c_str());
    font_text_on_screen_0->print_text();
    current_display->show(font_text_on_screen_0, current_x_anchor, current_y_anchor);
    delete font_text_on_screen_0;

    TextualFrameBuffer *font_text_on_screen_1 = new TextualFrameBuffer(test_string.size(), 1, {.font = current_font[1]});
    current_x_anchor = 64;
    current_y_anchor = 8;
    sprintf(font_text_on_screen_1->text_buffer, test_string.c_str());
    font_text_on_screen_1->print_text();
    current_display->show(font_text_on_screen_1, current_x_anchor, current_y_anchor);
    delete font_text_on_screen_1;

    TextualFrameBuffer *font_text_on_screen_2 = new TextualFrameBuffer(test_string.size(), 1, {});
    font_text_on_screen_2->update_font(current_font[2]);
    current_x_anchor = 0;
    current_y_anchor = 16;
    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->print_text();
    current_display->show(font_text_on_screen_2, current_x_anchor, current_y_anchor);

    font_text_on_screen_2->update_font(current_font[3]);
    current_x_anchor = 64;
    current_y_anchor = 32;
    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->print_text();
    current_display->show(font_text_on_screen_2, current_x_anchor, current_y_anchor);

    sleep_ms(1000);
}
void test_full_screen_text(SSD1306 *current_display)
{
    struct_TextFramebuffer txt_conf = {
        .font = font_8x8,
        .wrap = true,
    };
    TextualFrameBuffer text_frame = TextualFrameBuffer(SSD1306_WIDTH, SSD1306_HEIGHT, FramebufferFormat::MONO_VLSB, txt_conf);

    text_frame.print_char(FORM_FEED); // equiv. clear full screen
    current_display->show(&text_frame, 0, 0);
    uint16_t nb = text_frame.char_height * text_frame.char_width;

    uint16_t n{0};
    for (uint16_t c = 32; c < 256; c++)
    {
        n++;
        text_frame.print_char(c);
        current_display->show(&text_frame, 0, 0);
        if (n == nb)
        {
            sleep_ms(500);
            text_frame.print_char(FORM_FEED);
        }
    }
    sleep_ms(1000);
}
void test_auto_next_char(SSD1306 *current_display)
{
    struct_TextFramebuffer txt_conf = {
        .font = font_8x8,
        .wrap = true,
        .auto_next_char = false};

    TextualFrameBuffer * text_frame = new TextualFrameBuffer(SSD1306_WIDTH, SSD1306_HEIGHT, FramebufferFormat::MONO_VLSB, txt_conf);

    text_frame->print_char(FORM_FEED);

    uint16_t n{0};
    for (uint16_t c = 32; c < 128; c++)
    {
        n++;
        text_frame->print_char(c);
        current_display->show(text_frame,0,0);
        if (n % 5 == 0)
        {
            text_frame->next_char();
        }
    }
    sleep_ms(1000);
}

int main()

{
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 left_display = SSD1306(&master, cfg_left_screen);
    SSD1306 right_display = SSD1306(&master, cfg_right_screen);

    while (true)
    {
        test_font_size(&left_display);
        test_full_screen_text(&right_display);
        test_auto_next_char(&left_display);
        // test_sprintf_format(&left_display);
        // test_ostringstream_format(&left_display);
        // test_text_and_graph(&left_display);
    }
}

// void test_ostringstream_format(SSD1306 *left_display)
//  {
//      pr_D4.hi();
//      left_display->clear_full_screen();
//      pr_D4.lo(); // 25.74 ms

//     const unsigned char *current_font{font_5x8};

//     struct_FramebufferText txt_conf = {
//         .font = current_font,
//         .wrap = false};
//     left_display->update_text_buffer(txt_conf);

//     int n = 42;
//     float f = std::numbers::pi;

//     std::ostringstream stream0, stream1, stream2;

//     stream0.fill('.');
//     stream2.fill('.');
//     stream2.precision(4);
//     stream2.width(20);

//     pr_D5.hi(); // 1.5 ms since  pr_D4.lo();
//     stream0 << std::left << std::setw(6) << "test" << std::endl;
//     left_display->print_text(stream0.str().c_str());

//     pr_D7.hi();
//     left_display->show();
//     sleep_ms(500);
//     pr_D7.lo(); // 25.77 ms

//     stream1 << std::setw(5) << std::dec << n << "|" << std::setw(5)
//             << std::showbase << std::hex << n << "|" << std::showbase << std::setw(5) << std::oct << n << std::endl;
//     left_display->print_text(stream1.str().c_str());

//     pr_D7.hi();
//     left_display->show();
//     sleep_ms(500);
//     pr_D7.lo(); // 25.77 ms

//     stream2 << "PI = " << std::left << f << std::endl;
//     left_display->print_text(stream2.str().c_str());
//     pr_D5.lo(); // 1.246 ms

//     pr_D7.hi();
//     left_display->show();
//     pr_D7.lo(); // 25.77 ms

//     sleep_ms(2000);
// }

// void test_sprintf_format(SSD1306 *left_display)
// {
//     left_display->clear_full_screen();

//     struct_FramebufferText cfg_fb_txt = {
//         .font = font_8x8,
//         .wrap = false};
//     left_display->update_text_buffer(cfg_fb_txt);

//     const char *s = "Hello";

//     left_display->print_text("Strings:\n\tpadding:\n");
//     left_display->show();
//     sprintf(left_display->text_buffer, "\t[%7s]\n", s);
//     left_display->print_text();
//     left_display->show();
//     sprintf(left_display->text_buffer, "\t[%-7s]\n", s);
//     left_display->print_text();
//     left_display->show();
//     sprintf(left_display->text_buffer, "\t[%*s]\n", 7, s);
//     left_display->print_text();
//     left_display->show();
//     left_display->print_text("\ttruncating:\n");
//     left_display->show();
//     sprintf(left_display->text_buffer, "\t%.4s\n", s);
//     left_display->print_text();
//     left_display->show();
//     sprintf(left_display->text_buffer, "\t\t%.*s\n", 3, s);
//     left_display->print_text();
//     left_display->show();
//     sleep_ms(2000);

//     left_display->clear_pixel_buffer();
//     left_display->clear_text_buffer();
//     sprintf(left_display->text_buffer, "Characters: %c %%", 'A');
//     left_display->print_text();
//     left_display->show();
//     sleep_ms(2000);

//     left_display->clear_pixel_buffer();
//     left_display->set_font(font_5x8);
//     left_display->clear_text_buffer();

//     left_display->print_text("Integers:\n");
//     sprintf(left_display->text_buffer, "\tDec:  %i %d %.3i %i %.0i %+i %i\n", 1, 2, 3, 0, 0, 4, -4);
//     left_display->print_text();
//     sprintf(left_display->text_buffer, "\tHex:  %x %x %X %#x\n", 5, 10, 10, 6);
//     left_display->print_text();
//     sprintf(left_display->text_buffer, "\tOct:    %o %#o %#o\n", 10, 10, 4);
//     left_display->print_text();
//     left_display->print_text("Floating point:\n");
//     sprintf(left_display->text_buffer, "\tRnd:  %f %.0f %.3f\n", 1.5, 1.5, 1.5);
//     left_display->print_text();
//     sprintf(left_display->text_buffer, "\tPad:  %05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
//     left_display->print_text();
//     sprintf(left_display->text_buffer, "\tSci:  %.3E %.1e\n", 1.5, 1.5);
//     left_display->print_text();
//     left_display->show();
//     sleep_ms(2000);

//     left_display->clear_pixel_buffer();

// #define DELAY 500

//     left_display->set_font(font_8x8);
//     pr_D4.hi();
//     left_display->print_text(" !\"#$%&'()*+,-./0123456789:;<=>?\n"); // ca 1000us -> 2000us
//     pr_D4.lo();
//     left_display->show();
//     sleep_ms(DELAY);
//     pr_D4.hi();
//     left_display->print_text("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\n"); // ca 1000us -> 2000us
//     pr_D4.lo();
//     left_display->show();
//     sleep_ms(DELAY);
//     pr_D4.hi();
//     left_display->print_text("`abcdefghijklmnopqrstuvwxyz{|}~\x7F\n"); // ca 1000us-> 2000us
//     pr_D4.lo();
//     left_display->show();
//     sleep_ms(DELAY);
//     pr_D4.hi();
//     left_display->print_text("1234567890\n"); // ca 400us -> 800us
//     pr_D4.lo();
//     left_display->show();
//     sleep_ms(DELAY);
//     pr_D4.hi();
//     left_display->print_text("\t1234567890\n"); // ca 400us -> 800us
//     pr_D4.lo();
//     left_display->show();
//     sleep_ms(DELAY);
//     pr_D4.hi();
//     left_display->print_text("\t\tABCD\t\n"); // ca 400us -> 800us
//     pr_D4.lo();
//     left_display->show();
//     sleep_ms(DELAY);
//     pr_D4.hi();
//     left_display->print_text("\t\t\tABCD\n"); // ca 400us -> 800us
//     pr_D4.lo();
//     left_display->show();
//     sleep_ms(DELAY);
//     pr_D4.hi();
//     left_display->print_text("\t\t\t\tABCD\n"); // ca 400us -> 800us
//     pr_D4.lo();
//     left_display->show();
//     sleep_ms(DELAY);
//     pr_D4.hi();
//     left_display->print_text("\t\t\t\t\tABCDE\n"); // ca 400us -> 800us
//     pr_D4.lo();
//     left_display->show();
//     sleep_ms(DELAY);
//     pr_D4.hi();
//     left_display->print_text("\t\t\t\tABCDEF\t\n"); // ca 400us -> 800us
//     pr_D4.lo();

//     left_display->show();
//     sleep_ms(2000);
//     left_display->clear_pixel_buffer();

//     left_display->set_font(font_12x16);
//     left_display->print_text("090\b:56\n");
//     left_display->print_text("03 JAN 24");
//     left_display->show();
//     sleep_ms(2000);
//     left_display->clear_pixel_buffer();

//     left_display->set_font(font_16x32);
//     left_display->print_text(" 15:06 \n");
//     left_display->print_text("03/01/24");
//     left_display->show();
//     sleep_ms(2000);
//     left_display->clear_pixel_buffer();

//     /*
//     undefined result for the used compiler
//     printf("\tHexadecimal:\t%a %A\n", 1.5, 1.5);
//     printf("\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0 / 0.0, 1.0 / 0.0);
//     printf("Fixed-width types:\n");
//     printf("\tLargest 32-bit value is %" PRIu32 " or %#" PRIx32 "\n",
//            UINT32_MAX, UINT32_MAX);
//     */
// }

// void test_text_and_graph(SSD1306 *left_display)
// {
// #define DEGREE "\xF8"
//     left_display->clear_full_screen();
//     struct_FramebufferText title_config = {
//         .font = font_8x8};
//     uint8_t w = title_config.font[FONT_WIDTH_INDEX];
//     uint8_t h = title_config.font[FONT_HEIGHT_INDEX];

//     int title_area_width = w * 8;
//     int title_area_height = h * 2;
//     int title_area_anchor_x = 0;
//     int title_area_anchor_y = h * 6;

//     Framebuffer title = Framebuffer(title_area_width, title_area_height);
//     title.update_text_buffer(title_config);
//     title.print_text("ROLL:\nPITCH:");
//     left_display->show(&title, title_area_anchor_x, title_area_anchor_y);

//     // draw values
//     int values_area_anchor_x = w * 8;
//     int values_area_anchor_y = h * 6;
//     int values_area_width = w * 8;
//     int values_area_height = h * 2;
//     Framebuffer values = Framebuffer(values_area_width, values_area_height);
//     values.set_font(font_8x8);

//     // draw graph
//     int graph_area_anchor_x = 16;
//     int graph_area_anchor_y = 0;
//     int graph_area_width = w * 12;
//     int graph_area_height = h * 5;

//     Framebuffer graph = Framebuffer(graph_area_width, graph_area_height);
//     graph.fill(FramebufferColor::BLACK);
//     left_display->show(&graph, graph_area_anchor_x, graph_area_anchor_y);

//     int roll, pitch;
//     char *c_str = new char[2 * values.max_line + 1];

//     for (int i = -90; i < 90; i++)
//     {
//         values.clear_pixel_buffer();
//         roll = i;
//         pitch = i / 3;
//         sprintf(c_str, "%+3d \xF8\n%+3d \xF8", roll, pitch);
//         values.print_text(c_str);
//         left_display->show(&values, values_area_anchor_x, values_area_anchor_y);

//         float xc = graph_area_width / 2;
//         float yc = graph_area_height / 2;
//         float yl = graph_area_height / 2 - pitch;
//         float radius = yc - 2; // radius -2 to fit inside the rectangle

//         float sin_roll = sin(std::numbers::pi / 180.0 * roll);
//         float cos_roll = cos(std::numbers::pi / 180.0 * roll);
//         int x0 = xc - radius * cos_roll;
//         int y0 = yl - radius * sin_roll;
//         int x1 = xc + radius * cos_roll;
//         int y1 = yl + radius * sin_roll;
//         graph.rect(0, 0, graph_area_width, graph_area_height); // point coordinates are relative to the local frame
//         graph.circle(radius, xc, yl);
//         graph.line(x0, y0, x1, y1);
//         left_display->show(&graph, graph_area_anchor_x, graph_area_anchor_y);
//         left_display->show(&values, values_area_anchor_x, values_area_anchor_y);
//         graph.line(x0, y0, x1, y1, FramebufferColor::BLACK);
//         graph.circle(radius, xc, yl, false, FramebufferColor::BLACK);
//         sleep_ms(50);
//     }
//     delete[] c_str;
//     sleep_ms(1000);
// }
