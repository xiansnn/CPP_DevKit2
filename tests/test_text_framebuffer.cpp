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
    font_text_on_screen_2->update_pixel_area(current_font[2]);
    current_x_anchor = 0;
    current_y_anchor = 16;
    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->print_text();
    current_display->show(font_text_on_screen_2, current_x_anchor, current_y_anchor);

    font_text_on_screen_2->update_pixel_area(current_font[3]);
    current_x_anchor = 64;
    current_y_anchor = 32;
    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->print_text();
    current_display->show(font_text_on_screen_2, current_x_anchor, current_y_anchor);
    delete font_text_on_screen_2;

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_full_screen();
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

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_full_screen();
}

void test_auto_next_char(SSD1306 *current_display)
{
    struct_TextFramebuffer txt_conf = {
        .font = font_8x8,
        .wrap = true,
        .auto_next_char = false};

    TextualFrameBuffer *text_frame = new TextualFrameBuffer(SSD1306_WIDTH, SSD1306_HEIGHT, FramebufferFormat::MONO_VLSB, txt_conf);

    text_frame->print_char(FORM_FEED);

    uint16_t n{0};
    for (uint16_t c = 32; c < 128; c++)
    {
        n++;
        text_frame->print_char(c);
        current_display->show(text_frame, 0, 0);
        if (n % 5 == 0)
        {
            text_frame->next_char();
        }
    }

    delete text_frame;

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_full_screen();
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
    current_display->clear_full_screen();

    struct_TextFramebuffer text_frame_cfg = {
        .font = font_8x8,
        .wrap = true};

    TextualFrameBuffer *text_frame = new TextualFrameBuffer(SSD1306_WIDTH, SSD1306_HEIGHT, FramebufferFormat::MONO_VLSB, text_frame_cfg);

    const char *s = "Hello";

    text_frame->print_text("Strings:\n\tpadding:\n");
    current_display->show(text_frame, 0, 0);

    sprintf(text_frame->text_buffer, "\t[%7s]\n", s);
    text_frame->print_text();
    current_display->show(text_frame, 0, 0);
    sprintf(text_frame->text_buffer, "\t[%-7s]\n", s);
    text_frame->print_text();
    current_display->show(text_frame, 0, 0);
    sprintf(text_frame->text_buffer, "\t[%*s]\n", 7, s);
    text_frame->print_text();
    current_display->show(text_frame, 0, 0);
    text_frame->print_text("\ttruncating:\n");
    current_display->show(text_frame, 0, 0);
    sprintf(text_frame->text_buffer, "\t%.4s\n", s);
    text_frame->print_text();
    current_display->show(text_frame, 0, 0);
    sprintf(text_frame->text_buffer, "\t\t%.*s\n", 3, s);
    text_frame->print_text();
    current_display->show(text_frame, 0, 0);
    sleep_ms(LONG_DELAY);

    current_display->clear_full_screen();
    text_frame->clear_text_buffer();
    sprintf(text_frame->text_buffer, "Characters: %c %%", 'A');
    text_frame->print_text();
    current_display->show();
    sleep_ms(LONG_DELAY);

    current_display->clear_full_screen();
    text_frame->update_text_area(font_5x8);
    text_frame->clear_text_buffer();

    text_frame->print_text("Integers:\n");
    sprintf(text_frame->text_buffer, "\tDec:  %i %d %.3i %i %.0i %+i %i\n", 1, 2, 3, 0, 0, 4, -4);
    text_frame->print_text();
    sprintf(text_frame->text_buffer, "\tHex:  %x %x %X %#x\n", 5, 10, 10, 6);
    text_frame->print_text();
    sprintf(text_frame->text_buffer, "\tOct:    %o %#o %#o\n", 10, 10, 4);
    text_frame->print_text();
    text_frame->print_text("Floating point:\n");
    sprintf(text_frame->text_buffer, "\tRnd:  %f %.0f %.3f\n", 1.5, 1.5, 1.5);
    text_frame->print_text();
    sprintf(text_frame->text_buffer, "\tPad:  %05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
    text_frame->print_text();
    sprintf(text_frame->text_buffer, "\tSci:  %.3E %.1e\n", 1.5, 1.5);
    text_frame->print_text();
    current_display->show(text_frame, 0, 0);
    sleep_ms(LONG_DELAY);

    current_display->clear_full_screen();

    text_frame->update_text_area(font_8x8);

    text_frame->print_char(FORM_FEED); // equivalent text_frame->clear_pixel_buffer();

    text_frame->print_text(" !\"#$%&'()*+,-./0123456789:;<=>?");   // ca 1000us -> 2000us
    text_frame->print_text("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");   // ca 1000us -> 2000us
    text_frame->print_text("`abcdefghijklmnopqrstuvwxyz{|}~\x7F"); // ca 1000us-> 2000us
    text_frame->print_text("1234567890\n");                        // ca 400us -> 800us
    current_display->show(text_frame, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_char(FORM_FEED);

    text_frame->print_text("\t1TAB\n"); // ca 400us -> 800us
    current_display->show(text_frame, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_text("\t\t2TAB\n"); // ca 400us -> 800us
    current_display->show(text_frame, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_text("\t\t\t3TAB\n"); // ca 400us -> 800us
    current_display->show(text_frame, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_text("\t\t\t\t4TAB\n"); // ca 400us -> 800us
    current_display->show(text_frame, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_text("\t\t\t\t\t5TAB\n"); // ca 400us -> 800us
    current_display->show(text_frame, 0, 0);
    sleep_ms(DELAY);

    text_frame->print_text("\t1TAB\t\t\t3TAB\n"); // ca 400us -> 800us
    current_display->show(text_frame, 0, 0);

    sleep_ms(LONG_DELAY);
    text_frame->print_char(FORM_FEED);

    text_frame->update_text_area(font_16x32);
    text_frame->print_text(" 15:06 \n");
    text_frame->print_text("03/01/24");
    current_display->show(text_frame, 0, 0);

    sleep_ms(LONG_DELAY);

    delete text_frame;
    current_display->clear_full_screen();

    text_frame_cfg = {
        .font = font_12x16,
        .wrap = true};
    TextualFrameBuffer *text_frame2 = new TextualFrameBuffer(7, 2, text_frame_cfg);

    text_frame2->print_text(" 090\b:56\n"); // test effect of BACKSPACE
    text_frame2->print_text("03JAN24");
    current_display->show(text_frame2, 22, 16);
    delete text_frame2;

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_full_screen();
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

    current_display->clear_full_screen();

    const unsigned char *current_font{font_5x8};

    struct_TextFramebuffer txt_conf = {
        .font = current_font,
        .wrap = false};

    TextualFrameBuffer text_frame = TextualFrameBuffer(SSD1306_WIDTH, SSD1306_HEIGHT, FramebufferFormat::MONO_VLSB, txt_conf);

    int n = 42;
    float f = std::numbers::pi;

    std::ostringstream stream0, stream1, stream2;

    stream0.fill('.');
    stream2.fill('.');
    stream2.precision(4);
    stream2.width(20);

    stream0 << std::left << std::setw(6) << "test" << std::endl;
    text_frame.print_text(stream0.str().c_str());

    current_display->show(&text_frame, 0, 0);
    sleep_ms(DELAY);

    stream1 << std::setw(5) << std::dec << n << "|" << std::setw(5)
            << std::showbase << std::hex << n << "|" << std::showbase << std::setw(5) << std::oct << n << std::endl;
    text_frame.print_text(stream1.str().c_str());
    current_display->show(&text_frame, 0, 0);

    sleep_ms(DELAY);

    stream2 << "PI = " << std::left << f << std::endl;
    text_frame.print_text(stream2.str().c_str());
    current_display->show(&text_frame, 0, 0);

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_full_screen();
}

void test_text_and_graph(SSD1306 *current_display)
{
#define DEGREE "\xF8"
    current_display->clear_full_screen();
    struct_TextFramebuffer title_config = {
        .font = font_8x8};

    uint8_t w = title_config.font[FONT_WIDTH_INDEX];
    uint8_t h = title_config.font[FONT_HEIGHT_INDEX];

    int title_char_width = 8;
    int title_char_height = 2;
    int title_area_anchor_x = 0;
    int title_area_anchor_y = h * 6;

    TextualFrameBuffer title = TextualFrameBuffer(title_char_width, title_char_height, title_config);
    title.print_text("ROLL:\nPITCH:");
    current_display->show(&title, title_area_anchor_x, title_area_anchor_y);

    // draw values
    int values_area_anchor_x = w * 8;
    int values_area_anchor_y = h * 6;
    int values_char_width = 8;
    int values_char_height = 2;

    title_config.font = font_8x8;
    TextualFrameBuffer values = TextualFrameBuffer(values_char_width, values_char_height, title_config);
    values.print_char(FORM_FEED);

    // draw graph
    int graph_area_anchor_x = 16;
    int graph_area_anchor_y = 0;
    int graph_area_width = w * 12;
    int graph_area_height = h * 5;

    Framebuffer graph = Framebuffer(graph_area_width, graph_area_height);
    graph.clear_pixel_buffer();

    current_display->show(&graph, graph_area_anchor_x, graph_area_anchor_y);

    int roll, pitch;

    for (int i = -90; i < 90; i++)
    {
        // compute and show values
        roll = i;
        pitch = i / 3;
        sprintf(values.text_buffer, "%+3d \xF8\n%+3d \xF8", roll, pitch);
        values.print_text();
        current_display->show(&values, values_area_anchor_x, values_area_anchor_y);
        values.print_char(FORM_FEED);

        // compute and show the graphic representation
        float xc = graph_area_width / 2;
        float yc = graph_area_height / 2;
        float yl = graph_area_height / 2 - pitch;
        float radius = yc - 2; // radius -2 to fit inside the rectangle
        float sin_roll = sin(std::numbers::pi / 180.0 * roll);
        float cos_roll = cos(std::numbers::pi / 180.0 * roll);
        int x0 = xc - radius * cos_roll;
        int y0 = yl - radius * sin_roll;
        int x1 = xc + radius * cos_roll;
        int y1 = yl + radius * sin_roll;
        graph.rect(0, 0, graph_area_width, graph_area_height); // point coordinates are relative to the local frame
        graph.circle(radius, xc, yl);
        graph.line(x0, y0, x1, y1);
        current_display->show(&graph, graph_area_anchor_x, graph_area_anchor_y);
        graph.line(x0, y0, x1, y1, graph.frame_graph_config.bg_color);
        graph.circle(radius, xc, yl, false, graph.frame_graph_config.bg_color);

        sleep_ms(REFRESH_PERIOD);
    }

    sleep_ms(INTER_TEST_DELAY);

    current_display->clear_full_screen();
}

int main()

{
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 left_display = SSD1306(&master, cfg_left_screen);
    SSD1306 right_display = SSD1306(&master, cfg_right_screen);
    left_display.clear_full_screen();
    right_display.clear_full_screen();

    while (true)
    {
        test_font_size(&left_display);
        test_full_screen_text(&right_display);
        test_auto_next_char(&left_display);
        test_sprintf_format(&right_display);
        test_ostringstream_format(&left_display);
        test_text_and_graph(&right_display);
    }
}
