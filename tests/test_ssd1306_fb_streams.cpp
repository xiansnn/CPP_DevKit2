/**
 * @file test_streams.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "probe.h"
#include "hw_i2c.h"
#include "ssd1306.h"
#include "framebuffer.h"

#include <pico/stdio.h>
#include <math.h>
#include <numbers>
#include <sstream>
#include <iomanip>
#include <string>

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};

struct_ConfigSSD1306 cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

void test_ostringstream_format(SSD1306 *display)
{
    pr_D4.hi();
    display->clear_full_screen();
    pr_D4.lo(); // 25.74 ms

    const unsigned char *current_font{font_5x8};

    struct_FramebufferText txt_conf = {
        .font = current_font,
        .wrap = false};
    display->init_text_buffer(txt_conf);

    int n = 42;
    float f = std::numbers::pi;

    std::ostringstream stream0, stream1, stream2;

    stream0.fill('.');
    stream2.fill('.');
    stream2.precision(4);
    stream2.width(20);

    pr_D5.hi(); // 1.5 ms since  pr_D4.lo();
    stream0 << std::left << std::setw(6) << "test" << std::endl;
    display->print_text(stream0.str().c_str());

    pr_D7.hi();
    display->show();
    sleep_ms(500);
    pr_D7.lo(); // 25.77 ms

    stream1 << std::setw(5) << std::dec << n << "|" << std::setw(5)
            << std::showbase << std::hex << n << "|" << std::showbase << std::setw(5) << std::oct << n << std::endl;
    display->print_text(stream1.str().c_str());

    pr_D7.hi();
    display->show();
    sleep_ms(500);
    pr_D7.lo(); // 25.77 ms

    stream2 << "PI = " << std::left << f << std::endl;
    display->print_text(stream2.str().c_str()); 
    pr_D5.lo(); // 1.246 ms

    pr_D7.hi();
    display->show();
    pr_D7.lo(); // 25.77 ms

    sleep_ms(2000);
}


void test_sprintf_format(SSD1306 *display)
{
    display->clear_full_screen();

    struct_FramebufferText cfg_fb_txt = {
        .font = font_8x8,
        .wrap = false};
    display->init_text_buffer(cfg_fb_txt);

    const char *s = "Hello";

    display->print_text("Strings:\n\tpadding:\n");
    display->show();
    sprintf(display->text_buffer, "\t[%7s]\n", s);
    display->print_text();
    display->show();
    sprintf(display->text_buffer, "\t[%-7s]\n", s);
    display->print_text();
    display->show();
    sprintf(display->text_buffer, "\t[%*s]\n", 7, s);
    display->print_text();
    display->show();
    display->print_text("\ttruncating:\n");
    display->show();
    sprintf(display->text_buffer, "\t%.4s\n", s);
    display->print_text();
    display->show();
    sprintf(display->text_buffer, "\t\t%.*s\n", 3, s);
    display->print_text();
    display->show();
    sleep_ms(2000);

    display->clear_pixel_buffer();
    display->clear_text_buffer();
    sprintf(display->text_buffer, "Characters: %c %%", 'A');
    display->print_text();
    display->show();
    sleep_ms(2000);

    display->clear_pixel_buffer();
    display->set_font(font_5x8);
    display->clear_text_buffer();

    display->print_text("Integers:\n");
    sprintf(display->text_buffer, "\tDec:  %i %d %.3i %i %.0i %+i %i\n", 1, 2, 3, 0, 0, 4, -4);
    display->print_text();
    sprintf(display->text_buffer, "\tHex:  %x %x %X %#x\n", 5, 10, 10, 6);
    display->print_text();
    sprintf(display->text_buffer, "\tOct:    %o %#o %#o\n", 10, 10, 4);
    display->print_text();
    display->print_text("Floating point:\n");
    sprintf(display->text_buffer, "\tRnd:  %f %.0f %.3f\n", 1.5, 1.5, 1.5);
    display->print_text();
    sprintf(display->text_buffer, "\tPad:  %05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
    display->print_text();
    sprintf(display->text_buffer, "\tSci:  %.3E %.1e\n", 1.5, 1.5);
    display->print_text();
    display->show();
    sleep_ms(2000);

    display->clear_pixel_buffer();

#define DELAY 500

    display->set_font(font_8x8);
    pr_D4.hi();
    display->print_text(" !\"#$%&'()*+,-./0123456789:;<=>?\n"); // ca 1000us -> 2000us
    pr_D4.lo();
    display->show();
    sleep_ms(DELAY);
    pr_D4.hi();
    display->print_text("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\n"); // ca 1000us -> 2000us
    pr_D4.lo();
    display->show();
    sleep_ms(DELAY);
    pr_D4.hi();
    display->print_text("`abcdefghijklmnopqrstuvwxyz{|}~\x7F\n"); // ca 1000us-> 2000us
    pr_D4.lo();
    display->show();
    sleep_ms(DELAY);
    pr_D4.hi();
    display->print_text("1234567890\n"); // ca 400us -> 800us
    pr_D4.lo();
    display->show();
    sleep_ms(DELAY);
    pr_D4.hi();
    display->print_text("\t1234567890\n"); // ca 400us -> 800us
    pr_D4.lo();
    display->show();
    sleep_ms(DELAY);
    pr_D4.hi();
    display->print_text("\t\tABCD\t\n"); // ca 400us -> 800us
    pr_D4.lo();
    display->show();
    sleep_ms(DELAY);
    pr_D4.hi();
    display->print_text("\t\t\tABCD\n"); // ca 400us -> 800us
    pr_D4.lo();
    display->show();
    sleep_ms(DELAY);
    pr_D4.hi();
    display->print_text("\t\t\t\tABCD\n"); // ca 400us -> 800us
    pr_D4.lo();
    display->show();
    sleep_ms(DELAY);
    pr_D4.hi();
    display->print_text("\t\t\t\t\tABCDE\n"); // ca 400us -> 800us
    pr_D4.lo();
    display->show();
    sleep_ms(DELAY);
    pr_D4.hi();
    display->print_text("\t\t\t\tABCDEF\t\n"); // ca 400us -> 800us
    pr_D4.lo();

    display->show();
    sleep_ms(2000);
    display->clear_pixel_buffer();

    display->set_font(font_12x16);
    display->print_text("090\b:56\n");
    display->print_text("03 JAN 24");
    display->show();
    sleep_ms(2000);
    display->clear_pixel_buffer();

    display->set_font(font_16x32);
    display->print_text(" 15:06 \n");
    display->print_text("03/01/24");
    display->show();
    sleep_ms(2000);
    display->clear_pixel_buffer();


    /*
    undefined result for the used compiler
    printf("\tHexadecimal:\t%a %A\n", 1.5, 1.5);
    printf("\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0 / 0.0, 1.0 / 0.0);
    printf("Fixed-width types:\n");
    printf("\tLargest 32-bit value is %" PRIu32 " or %#" PRIx32 "\n",
           UINT32_MAX, UINT32_MAX);
    */
}

void test_text_and_graph(SSD1306 *display)
{
#define DEGREE "\xF8"
    display->clear_full_screen();
    struct_FramebufferText title_config = {
        .font = font_8x8};
    uint8_t w = title_config.font[FONT_WIDTH];
    uint8_t h = title_config.font[FONT_HEIGHT];

    int title_area_width = w * 8;
    int title_area_height = h * 2;
    int title_area_anchor_x = 0;
    int title_area_anchor_y = h * 6;

    Framebuffer title = Framebuffer(title_area_width, title_area_height);
    title.init_text_buffer(title_config);
    title.print_text("ROLL:\nPITCH:");
    display->show(&title, title_area_anchor_x, title_area_anchor_y);

    // draw values
    int values_area_anchor_x = w * 8;
    int values_area_anchor_y = h * 6;
    int values_area_width = w * 8;
    int values_area_height = h * 2;
    Framebuffer values = Framebuffer(values_area_width, values_area_height);
    values.set_font(font_8x8);

    // draw graph
    int graph_area_anchor_x = 16;
    int graph_area_anchor_y = 0;
    int graph_area_width = w * 12;
    int graph_area_height = h * 5;

    Framebuffer graph = Framebuffer(graph_area_width, graph_area_height);
    graph.fill(FramebufferColor::BLACK);
    display->show(&graph, graph_area_anchor_x, graph_area_anchor_y);

    int roll, pitch;
    char *c_str = new char[2 * values.max_line + 1];

    for (int i = -90; i < 90; i++)
    {
        values.clear_pixel_buffer();
        roll = i;
        pitch = i / 3;
        sprintf(c_str, "%+3d \xF8\n%+3d \xF8", roll, pitch);
        values.print_text(c_str);
        display->show(&values, values_area_anchor_x, values_area_anchor_y);

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
        display->show(&graph, graph_area_anchor_x, graph_area_anchor_y);
        display->show(&values, values_area_anchor_x, values_area_anchor_y);
        graph.line(x0, y0, x1, y1, FramebufferColor::BLACK);
        graph.circle(radius, xc, yl, false, FramebufferColor::BLACK);
        sleep_ms(50);
    }
    delete[] c_str;
    sleep_ms(1000);
}
void test_font_size(SSD1306 *display)
{
    display->clear_full_screen();
    const unsigned char *current_font[4]{font_5x8, font_8x8, font_12x16, font_16x32};
    char *c_str = new char[display->max_line + 1];
    sprintf(c_str, "Test");

    Framebuffer font_size_0 = Framebuffer(128, 8);
    font_size_0.set_font(current_font[0]);
    font_size_0.print_text(c_str);
    display->show(&font_size_0, 0, 0);

    Framebuffer *font_size_1 = new Framebuffer(64, 8);
    font_size_1->set_font(current_font[1]);
    font_size_1->print_text(c_str);
    display->show(font_size_1, 64, 8);
    delete font_size_1;

    Framebuffer *font_size_2 = new Framebuffer(64, 16);
    font_size_2->set_font(current_font[2]);
    font_size_2->print_text(c_str);
    display->show(font_size_2, 0, 16);
    delete font_size_2;

    Framebuffer font_size_3 = Framebuffer(64, 32);
    font_size_3.set_font(current_font[3]);
    font_size_3.print_text(c_str);
    display->show(&font_size_3, 64, 32);

    delete[] c_str;

    sleep_ms(1000);
}

void test_full_screen_text(SSD1306 *display)
{
    struct_FramebufferText txt_conf = {
        .font = font_8x8,
        .wrap = true,
    };
    display->init_text_buffer(txt_conf);

    display->print_char(FORM_FEED);
    uint16_t nb = display->max_column * display->max_line;
    uint16_t n{0};
    for (uint16_t c = 32; c < 256; c++)
    {
        n++;
        display->print_char(c);
        display->show();
        if (n == nb)
        {
            sleep_ms(500);
            display->print_char(FORM_FEED);
        }
    }
    sleep_ms(2000);
}

void test_auto_next_char(SSD1306 *display)
{
    struct_FramebufferText txt_conf = {
        .font = font_8x8,
        .wrap = true,
        .auto_next_char = false};
    display->init_text_buffer(txt_conf);
    display->print_char(FORM_FEED);

    uint16_t n{0};
    for (uint16_t c = 32; c < 128; c++)
    {
        n++;
        display->print_char(c);
        display->show();
        if (n % 5 == 0)
        {
            display->next_char();
        }
    }
    sleep_ms(2000);
}

int main()

{
    stdio_init_all();
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    while (true)
    {
        test_font_size(&display);
        test_sprintf_format(&display);
        test_ostringstream_format(&display);
        test_text_and_graph(&display);
        test_full_screen_text(&display);
        test_auto_next_char(&display);
    }
}
