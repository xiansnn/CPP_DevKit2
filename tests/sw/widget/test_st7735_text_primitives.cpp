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
#include "device/ST7735/st7735.h"

#include "utilities/probe/probe.h"
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

#define REFRESH_PERIOD 50
#define DELAY 500
#define LONG_DELAY 1000
#define INTER_TEST_DELAY 2000

// #define CANVAS_FORMAT CanvasFormat::RGB565_16b
#define CANVAS_FORMAT CanvasFormat::MONO_HMSB

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
    .rotation = DEVICE_DISPLAY_ROTATION};

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

void test_font_size(ST7735 *current_display)
{
    current_display->clear_device_screen_buffer();
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
    // font_text_on_screen_2->update_widget_anchor(64, 32);
    font_text_on_screen_2->update_widget_anchor(64, 32);
    sprintf(font_text_on_screen_2->text_buffer, test_string.c_str());
    font_text_on_screen_2->write();
    font_text_on_screen_2->show();
    delete font_text_on_screen_2;

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_device_screen_buffer();
}

void test_full_screen_text(ST7735 *current_display)
{
    struct_ConfigTextWidget txt_conf = {
        .font = font_8x8,
        .fg_color = ColorIndex::CYAN,
        .wrap = true,
    };
    my_text_widget text_frame = my_text_widget(current_display, txt_conf, CANVAS_FORMAT,
                                               current_display->TFT_panel_width_in_pixel, current_display->TFT_panel_height_in_pixel);

    text_frame.process_char(FORM_FEED); // equiv. clear full canvas, not the display RAM
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

void test_auto_next_char(ST7735 *current_display)
{
    struct_ConfigTextWidget txt_conf = {
        .font = font_8x8,
        .fg_color = ColorIndex::GOLD,
        .wrap = true,
        .auto_next_char = false};

    my_text_widget *text_frame = new my_text_widget(current_display, txt_conf, CANVAS_FORMAT,
                                                    current_display->TFT_panel_width_in_pixel, current_display->TFT_panel_width_in_pixel);

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

void test_sprintf_format(ST7735 *current_display)
{
    current_display->clear_device_screen_buffer();

    struct_ConfigTextWidget text_frame_cfg = {
        .font = font_8x8,
        .fg_color = ColorIndex::YELLOW,
        .wrap = true};

    my_text_widget *text_frame = new my_text_widget(current_display, text_frame_cfg, CANVAS_FORMAT,
                                                    current_display->TFT_panel_width_in_pixel, current_display->TFT_panel_width_in_pixel);

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

    sleep_ms(LONG_DELAY);

    current_display->clear_device_screen_buffer();
    text_frame->clear_text_buffer();
    sprintf(text_frame->text_buffer, "Characters: %c %%", 'A');
    text_frame->write();
    text_frame->show();

    sleep_ms(LONG_DELAY);

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

    sleep_ms(LONG_DELAY);

    current_display->clear_device_screen_buffer();

    text_frame->update_text_frame_size(font_8x8);

    text_frame->process_char(FORM_FEED);

    text_frame->write(" !\"#$%&'()*+,-./0123456789:;<=>?");   // ca 1000us -> 2000us
    text_frame->write("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");   // ca 1000us -> 2000us
    text_frame->write("`abcdefghijklmnopqrstuvwxyz{|}~\x7F"); // ca 1000us-> 2000us
    text_frame->write("1234567890\n");                        // ca 400us -> 800us
    text_frame->show();

    sleep_ms(LONG_DELAY);

    text_frame->process_char(FORM_FEED);

    text_frame->write("\t1TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(DELAY);

    text_frame->write("\t\t2TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(DELAY);

    text_frame->write("\t\t\t3TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(DELAY);

    text_frame->write("\t\t\t\t4TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(DELAY);

    text_frame->write("\t\t\t\t\t5TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(DELAY);

    text_frame->write("\t1TAB\t\t\t3TAB\n"); // ca 400us -> 800us
    text_frame->show();

    sleep_ms(LONG_DELAY);

    text_frame->process_char(FORM_FEED);
    text_frame->update_text_frame_size(font_16x32);

    text_frame->write(" 15:06 \n");
    text_frame->write("03/01/24");
    text_frame->show();

    sleep_ms(LONG_DELAY);

    current_display->clear_device_screen_buffer();

    delete text_frame;

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
    text_frame1->write(" 09:56 03JAN24");
    text_frame1->show();
    delete text_frame1;

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
    text_frame2->write(" 09:56\n03JAN24");
    text_frame2->show();
    delete text_frame2;

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
    text_frame3->write(" 09:56\n03JAN24");
    text_frame3->show();
    delete text_frame3;

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
    text_frame4->write(" 09:56\n03JAN24");
    text_frame4->show();
    delete text_frame4;

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
    text_frame5->write(" 09:56\n03JAN24");
    text_frame5->show();
    delete text_frame5;

    sleep_ms(INTER_TEST_DELAY);
    // current_display->clear_device_screen_buffer();
    /*
    undefined result for the used compiler
    printf("\tHexadecimal:\t%a %A\n", 1.5, 1.5);
    printf("\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0 / 0.0, 1.0 / 0.0);
    printf("Fixed-width types:\n");
    printf("\tLargest 32-bit value is %" PRIu32 " or %#" PRIx32 "\n",
           UINT32_MAX, UINT32_MAX);
    */
}

void test_ostringstream_format(ST7735 *current_display)
{

    current_display->clear_device_screen_buffer();

    const unsigned char *current_font{font_5x8};

    struct_ConfigTextWidget txt_conf = {
        .font = current_font,
        .fg_color = ColorIndex::MAGENTA,
        .wrap = false};

    my_text_widget text_frame = my_text_widget(current_display, txt_conf, CANVAS_FORMAT,
                                               current_display->TFT_panel_width_in_pixel, current_display->TFT_panel_width_in_pixel);

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
    sleep_ms(DELAY);

    stream1 << std::setw(5) << std::dec << n << "|" << std::setw(5)
            << std::showbase << std::hex << n << "|" << std::showbase << std::setw(5) << std::oct << n << std::endl;
    text_frame.write(stream1.str().c_str());
    current_display->show(text_frame.canvas, 0, 0);

    sleep_ms(DELAY);

    stream2 << "PI = " << std::left << f << std::endl;
    text_frame.write(stream2.str().c_str());
    current_display->show(text_frame.canvas, 0, 0);

    sleep_ms(INTER_TEST_DELAY);
    current_display->clear_device_screen_buffer();
}

void test_monochrome_canvas(ST7735 *display)
{
    display->clear_device_screen_buffer();

    std::string test_string = "\xB0\xB3\xB3\xB3\xB3";

    struct_ConfigTextWidget text_cfg{
        .number_of_column = (uint8_t)test_string.size(),
        .number_of_line = 1,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .font = font_5x8,
        .fg_color = ColorIndex::YELLOW,
        .bg_color = ColorIndex::RED};

    my_text_widget *mono_text = new my_text_widget(display, text_cfg, CANVAS_FORMAT);
    mono_text->write(test_string.c_str());
    mono_text->show();
    delete mono_text;

    sleep_ms(INTER_TEST_DELAY);

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

    my_text_widget text_frame = my_text_widget(display, txt_conf2, CANVAS_FORMAT);

    text_frame.process_char(FORM_FEED); 

    for (auto &&c : test_string)
    {
        text_frame.process_char(c);
        text_frame.show();
    }

    sleep_ms(INTER_TEST_DELAY);
}

int main()

{
    stdio_init_all();
    HW_SPI_Master spi_master = HW_SPI_Master(cfg_spi);
    ST7735 display = ST7735(&spi_master, cfg_st7735);
    display.clear_device_screen_buffer();

    while (true)
    {
        test_monochrome_canvas(&display);    //
        test_font_size(&display);            // 
        test_full_screen_text(&display);     // 
        test_auto_next_char(&display);       // 
        test_ostringstream_format(&display); // 
        test_sprintf_format(&display);       // 
    }
}
