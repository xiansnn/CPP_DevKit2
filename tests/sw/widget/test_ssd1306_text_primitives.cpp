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

#include "sw/widget/widget.h"
#include "hw/i2c/hw_i2c.h"
#include "device/SSD1306/ssd1306.h"
#include "utilities/probe/probe.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

#define REFRESH_PERIOD 50
#define DELAY_ms 500
#define LONG_DELAY_ms 1000
#define INTER_TEST_DELAY 2000

#define CANVAS_FORMAT CanvasFormat::MONO_VLSB

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

void test_font_size(SSD1306 *current_display)
{
    current_display->clear_device_screen_buffer();
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
    font_text_on_screen_0->show();
    delete font_text_on_screen_0;

    default_text_cfg.widget_anchor_x = 64;
    default_text_cfg.widget_anchor_y = 8;
    my_text_widget *font_text_on_screen_1 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    font_text_on_screen_1->update_canvas_buffer_size(current_font[1]);

    // process first text according to sprintf capabilities then copy to text buffer and finally draw text buffer into pixel buffer
    sprintf(font_text_on_screen_1->text_buffer, test_string.c_str());
    font_text_on_screen_1->write();
    font_text_on_screen_1->show();
    delete font_text_on_screen_1;

    default_text_cfg.widget_anchor_x = 0;
    default_text_cfg.widget_anchor_y = 16;
    my_text_widget *font_text_on_screen_2 = new my_text_widget(current_display, default_text_cfg, CANVAS_FORMAT);
    font_text_on_screen_2->update_canvas_buffer_size(current_font[2]);

    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->write();
    font_text_on_screen_2->show();

    font_text_on_screen_2->update_canvas_buffer_size(current_font[3]);
    font_text_on_screen_2->update_widget_anchor(64, 32);
    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->write();
    font_text_on_screen_2->show();
    delete font_text_on_screen_2;

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_device_screen_buffer();
}

void test_full_screen_text(SSD1306 *current_display)
{
    struct_ConfigTextWidget txt_conf = {
        .font = font_8x8,
        .wrap = true,
    };
    my_text_widget text_frame = my_text_widget(current_display, txt_conf, CANVAS_FORMAT, SSD1306_WIDTH, SSD1306_HEIGHT);

    text_frame.process_char(FORM_FEED); // equiv. clear full screen
    current_display->show(text_frame.canvas, 0, 0);
    uint16_t nb = text_frame.number_of_line * text_frame.number_of_column;

    uint16_t n{0};
    for (uint16_t c = 32; c < 256; c++)
    {
        n++;
        text_frame.process_char(c);
        current_display->show(text_frame.canvas, 0, 0);
        if (n == nb)
        {
            sleep_ms(500);
            text_frame.process_char(FORM_FEED);
        }
    }

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_device_screen_buffer();
}

void test_auto_next_char(SSD1306 *current_display)
{
    struct_ConfigTextWidget txt_conf = {
        .font = font_8x8,
        .wrap = true,
        .auto_next_char = false};

    my_text_widget *text_frame = new my_text_widget(current_display, txt_conf, CANVAS_FORMAT, SSD1306_WIDTH, SSD1306_HEIGHT);

    text_frame->process_char(FORM_FEED);

    uint16_t n{0};
    for (uint16_t c = 32; c < 128; c++)
    {
        n++;
        text_frame->process_char(c);
        current_display->show(text_frame->canvas, 0, 0);
        if (n % 8 == 0)
            text_frame->next_char();
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

    struct_ConfigTextWidget text_frame_cfg = {
        .font = font_8x8,
        .wrap = true};

    my_text_widget *text_frame = new my_text_widget(current_display, text_frame_cfg,CANVAS_FORMAT, SSD1306_WIDTH, SSD1306_HEIGHT);

    const char *s = "Hello";

    text_frame->write("Strings:\n\tpadding:\n");
    text_frame->show();

    sprintf(text_frame->text_buffer, "\t[%7s]\n", s);
    text_frame->write();
    text_frame->show();

    sprintf(text_frame->text_buffer, "\t[%-7s]\n", s);
    text_frame->write();
    text_frame->show();

    sprintf(text_frame->text_buffer, "\t[%*s]\n", 7, s);
    text_frame->write();
    text_frame->show();

    text_frame->write("\ttruncating:\n");
    text_frame->show();

    sprintf(text_frame->text_buffer, "\t%.4s\n", s);
    text_frame->write();
    text_frame->show();

    sprintf(text_frame->text_buffer, "\t\t%.*s\n", 3, s);
    text_frame->write();
    text_frame->show();

    sleep_ms(LONG_DELAY_ms);

    text_frame->show();

    text_frame->clear_text_buffer();
    // current_display->clear_pixel_buffer(&text_frame->pixel_frame);
    current_display->clear_device_screen_buffer();
    sprintf(text_frame->text_buffer, "Characters: %c %%", 'A');
    text_frame->write();
    text_frame->show();

    sleep_ms(LONG_DELAY_ms);

    current_display->clear_device_screen_buffer();

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
    text_frame->show();

    sleep_ms(LONG_DELAY_ms);

    current_display->clear_device_screen_buffer();

    text_frame->update_text_frame_size(font_8x8);

    text_frame->process_char(FORM_FEED); // equivalent text_frame->clear_pixel_buffer();

    text_frame->write(" !\"#$%&'()*+,-./0123456789:;<=>?");   // ca 1000us -> 2000us
    text_frame->write("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");   // ca 1000us -> 2000us
    text_frame->write("`abcdefghijklmnopqrstuvwxyz{|}~\x7F"); // ca 1000us-> 2000us
    text_frame->write("1234567890\n");                        // ca 400us -> 800us
    text_frame->show();

    sleep_ms(LONG_DELAY_ms);

    text_frame->process_char(FORM_FEED);

    text_frame->write("\t1TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(DELAY_ms);

    text_frame->write("\t\t2TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(DELAY_ms);

    text_frame->write("\t\t\t3TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(DELAY_ms);

    text_frame->write("\t\t\t\t4TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(DELAY_ms);

    text_frame->write("\t\t\t\t\t5TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(DELAY_ms);

    text_frame->write("\t1TAB\t\t\t3TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(LONG_DELAY_ms);

    text_frame->process_char(FORM_FEED);
    text_frame->update_text_frame_size(font_16x32);

    text_frame->write(" 15:06 \n");
    text_frame->write("03/01/24");
    text_frame->show();

    sleep_ms(LONG_DELAY_ms);

    current_display->clear_device_screen_buffer();

    delete text_frame;

    struct_ConfigTextWidget text_frame2_cfg = {
        .number_of_column = 7,
        .number_of_line = 2,
        .widget_anchor_x = 22,
        .widget_anchor_y = 16,
        .font = font_12x16,
        .wrap = false};
    my_text_widget *text_frame2 = new my_text_widget(current_display, text_frame2_cfg,CANVAS_FORMAT);

    text_frame2->write(" 09:56\n03JAN24");
    text_frame2->show();
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

    struct_ConfigTextWidget txt_conf = {
        .font = current_font,
        .wrap = false};

    my_text_widget text_frame = my_text_widget(current_display, txt_conf,CANVAS_FORMAT, SSD1306_WIDTH, SSD1306_HEIGHT);

    int n = 42;
    float f = std::numbers::pi;

    std::ostringstream stream0, stream1, stream2;

    stream0.fill('.');
    stream2.fill('.');
    stream2.precision(4);
    stream2.width(20);

    stream0 << std::left << std::setw(6) << "test" << std::endl;
    text_frame.write(stream0.str().c_str());

    current_display->show(text_frame.canvas, 0, 0);
    sleep_ms(DELAY_ms);

    stream1 << std::setw(5) << std::dec << n << "|" << std::setw(5)
            << std::showbase << std::hex << n << "|" << std::showbase << std::setw(5) << std::oct << n << std::endl;
    text_frame.write(stream1.str().c_str());
    current_display->show(text_frame.canvas, 0, 0);

    sleep_ms(DELAY_ms);

    stream2 << "PI = " << std::left << f << std::endl;
    text_frame.write(stream2.str().c_str());
    current_display->show(text_frame.canvas, 0, 0);

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
        test_ostringstream_format(&right_display);
        test_sprintf_format(&left_display);
    }
}
