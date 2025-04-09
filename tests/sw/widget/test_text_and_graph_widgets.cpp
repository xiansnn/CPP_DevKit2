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

#include "sw/widget/widget.h"
#include "hw/i2c/hw_i2c.h"
#include "device/SSD1306/ssd1306.h"
#include "utilities/probe/probe.h"

Probe pr_D1 = Probe(1);
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

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

class my_model : public Model
{
private:
    /* data */
public:
    my_model();
    ~my_model();
    int roll{0};
    int pitch{0};
    int cycle{0};
    void update_cycle(int i, int sign);
};

my_model::my_model()
{
}

my_model::~my_model()
{
}

void my_model::update_cycle(int i, int sign)
{
    this->roll = i;
    this->pitch = sign * i / 4;
    set_change_flag();
}

class my_text_widget : public TextWidget
{

public:
    my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg, Model *model = nullptr);
    my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg,
                   uint8_t x, uint8_t y, Model *model = nullptr);
    ~my_text_widget();
    void get_value_of_interest();
};
my_text_widget::my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg, Model *model)
    : TextWidget(graphic_display_screen, text_cfg, model) {}
my_text_widget::my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg,
                               uint8_t x, uint8_t y, Model *model)
    : TextWidget(graphic_display_screen, text_cfg, x, y, model) {}
my_text_widget::~my_text_widget() {}
void my_text_widget::get_value_of_interest()
{
    sprintf(this->text_buffer, "%+3d\xF8  %+3d\xF8", ((my_model *)this->actual_displayed_model)->roll, ((my_model *)this->actual_displayed_model)->pitch);
}

class my_graphic_widget : public GraphicWidget
{
private:
    int roll{0};
    int pitch{0};

public:
    my_graphic_widget(GraphicDisplayDevice *graphic_display_screen,
                      struct_ConfigGraphicWidget graph_cfg, Model *model);
    ~my_graphic_widget();
    void get_value_of_interest();
    void draw();
};
my_graphic_widget::my_graphic_widget(GraphicDisplayDevice *graphic_display_screen,
                                     struct_ConfigGraphicWidget graph_cfg, Model *model)
    : GraphicWidget(graphic_display_screen, graph_cfg, model) {}
my_graphic_widget::~my_graphic_widget() {}
void my_graphic_widget::get_value_of_interest()
{
    this->roll = ((my_model *)this->actual_displayed_model)->roll;
    this->pitch = ((my_model *)this->actual_displayed_model)->pitch;
}
void my_graphic_widget::draw()
{
    if (actual_displayed_model->has_changed())
    {
        clear_widget();
        get_value_of_interest();
    
        // compute and show the graphic representation
        float xc = widget_width / 2;
        float yc = widget_height / 2;
        float yl = widget_height / 2 - pitch;
        float radius = yc - 2 * widget_border_width; // radius -2 to fit inside the rectangle
        float sin_roll = sin(std::numbers::pi / 180.0 * roll);
        float cos_roll = cos(std::numbers::pi / 180.0 * roll);
        int x0 = xc - radius * cos_roll;
        int y0 = yl - radius * sin_roll;
        int x1 = xc + radius * cos_roll;
        int y1 = yl + radius * sin_roll;
    
        this->circle(radius, xc, yl, false, fg_color);
        this->line(x0, y0, x1, y1, fg_color);
    
        draw_border();
        show();
        actual_displayed_model->draw_widget_done();
    }
    
}

int main()

{
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 values_display = SSD1306(&master, cfg_left_screen);
    SSD1306 visu_display = SSD1306(&master, cfg_right_screen);
    values_display.clear_device_screen_buffer();
    visu_display.clear_device_screen_buffer();

    struct_ConfigTextWidget title_config = {
        .number_of_column = 10,
        .number_of_line = 1,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .font = font_12x16};

    uint8_t w = title_config.font[FONT_WIDTH_INDEX];
    uint8_t h = title_config.font[FONT_HEIGHT_INDEX];
    uint8_t values_anchor_y = 2 * h;

    struct_ConfigTextWidget values_config = {
        .number_of_column = 10,
        .number_of_line = 1,
        .widget_anchor_x = 0,
        .widget_anchor_y = values_anchor_y,
        .font = font_12x16};

    struct_ConfigGraphicWidget graph_config{
        .pixel_frame_width = 120,
        .pixel_frame_height = 56,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .widget_with_border = true};

    my_model model = my_model();

    my_text_widget values = my_text_widget(&values_display, values_config, &model);
    values.process_char(FORM_FEED);

    my_graphic_widget graph = my_graphic_widget(&visu_display, graph_config, &model);
    pr_D1.hi();
    visu_display.clear_pixel_buffer(&graph.pixel_frame);
    pr_D1.lo(); // 8µs

    pr_D1.hi();
    my_text_widget title = my_text_widget(&values_display, title_config);
    title.write("ROLL PITCH");
    title.show();
    pr_D1.lo(); // 9ms

    int sign = 1;

    while (true)
    {
        sign *= -1;
        for (int i = -90; i < 90; i++)
        {
            // compute and show values
            model.update_cycle(i, sign);
            pr_D4.hi();
            values.draw();
            pr_D4.lo(); // 9ms

            pr_D5.hi();
            graph.draw();
            pr_D5.lo(); // 21ms
            sleep_ms(REFRESH_PERIOD);
        }
    }
}
