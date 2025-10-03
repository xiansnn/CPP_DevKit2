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
#include "device/ST7735/st7735.h"

#include "utilities/probe/probe.h"
Probe pr_D1 = Probe(1);
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

#define REFRESH_PERIOD 20

#define DEGREE \xF8

#define GRAPHICS_CANVAS_FORMAT CanvasFormat::trueRGB565
#define TEXT_CANVAS_FORMAT CanvasFormat::MONO_HMSB

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
                   struct_ConfigTextWidget text_cfg, CanvasFormat format, Model *model = nullptr);
    my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg, CanvasFormat format,
                   uint8_t x, uint8_t y, Model *model = nullptr);
    ~my_text_widget();
    void get_value_of_interest();
};
my_text_widget::my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg, CanvasFormat format, Model *model)
    : TextWidget(graphic_display_screen, text_cfg, format, model) {}
my_text_widget::my_text_widget(GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg, CanvasFormat format,
                               uint8_t x, uint8_t y, Model *model)
    : TextWidget(graphic_display_screen, text_cfg, format, x, y, model) {}
my_text_widget::~my_text_widget() {}
void my_text_widget::get_value_of_interest()
{
    sprintf(this->text_buffer, "%+3d\xF8  %+3d\xF8", ((my_model *)this->actual_displayed_model)->roll, ((my_model *)this->actual_displayed_model)->pitch);
}

class my_visu_widget : public GraphicWidget
{
private:
    int roll{0};
    int pitch{0};

public:
    my_visu_widget(GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigGraphicWidget graph_cfg, CanvasFormat format, Model *model);
    ~my_visu_widget();
    void get_value_of_interest();
    void draw();
};
my_visu_widget::my_visu_widget(GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigGraphicWidget graph_cfg, CanvasFormat format, Model *model)
    : GraphicWidget(graphic_display_screen, graph_cfg, format, model) {}
my_visu_widget::~my_visu_widget() {}
void my_visu_widget::get_value_of_interest()
{
    this->roll = ((my_model *)this->actual_displayed_model)->roll;
    this->pitch = ((my_model *)this->actual_displayed_model)->pitch;
}
void my_visu_widget::draw()
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

        this->canvas->fill_canvas_with_color(bg_color);

        this->circle(radius, xc, yl, true, ColorIndex::GREEN);
        this->line(x0, y0, x1, y1, ColorIndex::YELLOW);

        draw_border(fg_color);
        show();
        actual_displayed_model->draw_widget_done();
    }
}

int main()

{
    stdio_init_all();
    HW_SPI_Master spi_master = HW_SPI_Master(cfg_spi);
    ST7735 display = ST7735(&spi_master, cfg_st7735);
    struct_ConfigTextWidget title_config = {
        .number_of_column = 10,
        .number_of_line = 1,
        .widget_anchor_x = 0,
        .widget_anchor_y = 64,
        .font = font_12x16};

    uint8_t w = title_config.font[FONT_WIDTH_INDEX];
    uint8_t h = title_config.font[FONT_HEIGHT_INDEX];

    struct_ConfigTextWidget values_config = {
        .number_of_column = 10,
        .number_of_line = 1,
        .widget_anchor_x = 0,
        .widget_anchor_y = (uint8_t)(title_config.widget_anchor_y + 2 * h),
        .font = font_12x16};

    struct_ConfigGraphicWidget graph_config{
        .pixel_frame_width = 128,
        .pixel_frame_height = 56,
        .fg_color = ColorIndex::CYAN,
        .bg_color = ColorIndex::MAROON,
        .widget_anchor_x = 0,
        .widget_anchor_y = 0,
        .widget_with_border = true};

    my_model model = my_model();

    my_text_widget values = my_text_widget(&display, values_config, TEXT_CANVAS_FORMAT, &model);
    values.process_char(FORM_FEED);

    my_visu_widget graph = my_visu_widget(&display, graph_config, GRAPHICS_CANVAS_FORMAT, &model);
    pr_D1.hi();
    display.clear_device_screen_buffer(); 
    pr_D1.lo();                           // 51ms

    pr_D1.hi();
    my_text_widget title = my_text_widget(&display, title_config, TEXT_CANVAS_FORMAT);
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
            pr_D4.lo(); // 9.9ms

            pr_D5.hi();
            graph.draw();
            pr_D5.lo(); // 29.6ms
            sleep_ms(REFRESH_PERIOD);
        }
    }
}
