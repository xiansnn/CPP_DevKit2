#include "t_rtos_st7735_text_and_graph_widgets.h"

#define DEGREE \xF8

my_model::my_model() {}
my_model::~my_model() {}

void my_model::update_cycle(int i, int sign)
{
    this->roll = i ;
    this->pitch = sign * i  / 4;

}

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

    this->canvas->fill_canvas_with_color(canvas->bg_color);

    this->circle(radius, xc, yl, true, ColorIndex::LIME);
    this->line(x0, y0, x1, y1, ColorIndex::RED);

    draw_border(canvas->fg_color);

}

void my_text_widget::draw()
{
    clear_text_buffer();
    get_value_of_interest();
    write();
    draw_border();
}
