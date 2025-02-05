/**
 * @file test_text_and_graph_framebuffer.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-02-05
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

#define REFRESH_PERIOD 20

#define DEGREE \xF8

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

int main()

{
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 values_display = SSD1306(&master, cfg_left_screen);
    SSD1306 visu_display = SSD1306(&master, cfg_right_screen);
    values_display.clear_full_screen();
    visu_display.clear_full_screen();

    struct_TextFramebufferConfig title_config = {
        .font = font_12x16};
    uint8_t w = title_config.font[FONT_WIDTH_INDEX];
    uint8_t h = title_config.font[FONT_HEIGHT_INDEX];
    int title_char_width = 10;
    int title_char_height = 1;
    int title_area_anchor_x = 0;
    int title_area_anchor_y = h * 0;

    struct_TextFramebufferConfig values_config = {
        .font = font_12x16};
    int values_char_width = 10;
    int values_char_height = 1;
    int values_area_anchor_x = w * 0;
    int values_area_anchor_y = h * 2;

    int graph_area_anchor_x = 0;
    int graph_area_anchor_y = 0;
    int graph_area_width = 120;
    int graph_area_height = 60;
    int roll, pitch;

    while (true)
    {
        pr_D4.hi();
        TextualFrameBuffer title = TextualFrameBuffer(&values_display, title_char_width, title_char_height, title_config);
        title.print_text("ROLL PITCH");
        values_display.show(&title.pixel_memory, title_area_anchor_x, title_area_anchor_y);
        pr_D4.lo();//9ms

        // draw values

        TextualFrameBuffer values = TextualFrameBuffer(&values_display, values_char_width, values_char_height, values_config);
        values.print_char(FORM_FEED);

        // draw graph

        GraphicFramebuffer graph = GraphicFramebuffer(&visu_display, graph_area_width, graph_area_height);
        visu_display.clear_pixel_buffer(&graph.pixel_memory);
        pr_D5.hi();
        visu_display.show(&graph.pixel_memory, graph_area_anchor_x, graph_area_anchor_y);
        pr_D5.lo();//24ms

        for (int i = -90; i < 90; i++)
        {
            // compute and show values
            pr_D6.hi();
            roll = i;
            pitch = i / 3;
            sprintf(values.text_buffer, "%+3d\xF8  %+3d\xF8", roll, pitch);
            values.print_text();
            values_display.show(&values.pixel_memory, values_area_anchor_x, values_area_anchor_y);
            values.print_char(FORM_FEED);
            pr_D6.lo();//9ms

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

            pr_D7.hi();
            graph.rect(0, 0, graph_area_width, graph_area_height); // point coordinates are relative to the local frame
            graph.circle(radius, xc, yl);
            graph.line(x0, y0, x1, y1);
            visu_display.show(&graph.pixel_memory, graph_area_anchor_x, graph_area_anchor_y);
            graph.line(x0, y0, x1, y1, graph.bg_color);
            graph.circle(radius, xc, yl, false, graph.bg_color);
            pr_D7.lo();//24ms

            sleep_ms(REFRESH_PERIOD);
        }

        values_display.clear_full_screen();
        visu_display.clear_full_screen();
    }
}
