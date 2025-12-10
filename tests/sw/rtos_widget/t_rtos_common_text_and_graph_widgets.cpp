#include "t_rtos_common_text_and_graph_widgets.h"

#define DEGREE \xF8

my_model::my_model() {}
my_model::~my_model() {}

void my_model::update_cycle(int i, int sign)
{
    this->roll = i;
    this->pitch = sign * i / 4;
}

my_text_widget::my_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model)
    : rtos_TextWidget(model, text_cfg, format, graphic_display_screen) {}

my_text_widget::my_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg, CanvasFormat format,
                               uint8_t x, uint8_t y, rtos_Model *model)
    : rtos_TextWidget(model, text_cfg, format, x, y, graphic_display_screen) {}

my_text_widget::~my_text_widget() {}

void my_text_widget::get_value_of_interest()
{
    sprintf(this->writer->text_buffer, "%+3d\xF8  %+3d\xF8", ((my_model *)this->actual_rtos_displayed_model)->roll, ((my_model *)this->actual_rtos_displayed_model)->pitch);
}

my_visu_widget::my_visu_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigGraphicWidget graph_cfg, CanvasFormat format, rtos_Model *model)
    : rtos_GraphicWidget(model, graph_cfg, format, graphic_display_screen) {}

my_visu_widget::~my_visu_widget() {}

void my_visu_widget::get_value_of_interest()
{
    this->roll = ((my_model *)this->actual_rtos_displayed_model)->roll;
    this->pitch = ((my_model *)this->actual_rtos_displayed_model)->pitch;
}

void my_visu_widget::draw()
{
    drawer->clear_widget();
    get_value_of_interest();

    // compute and show the graphic representation
    float xc =drawer->canvas->canvas_width_pixel / 2;
    float yc =drawer->canvas->canvas_height_pixel / 2;
    float yl = yc - pitch;
    float radius = yc - 2 * drawer->widget_border_width; // radius -2 to fit inside the rectangle
    float sin_roll = sin(std::numbers::pi / 180.0 * roll);
    float cos_roll = cos(std::numbers::pi / 180.0 * roll);
    int x0 = xc - radius * cos_roll;
    int y0 = yl - radius * sin_roll;
    int x1 = xc + radius * cos_roll;
    int y1 = yl + radius * sin_roll;

    this->drawer->canvas->fill_canvas_with_color(drawer->canvas->bg_color);

    this->drawer->circle(radius, xc, yl, true, ColorIndex::WHITE);
    this->drawer->line(x0, y0, x1, y1, ColorIndex::BLACK);

    drawer->draw_border(drawer->canvas->fg_color);
}

void my_text_widget::draw()
{
    this->writer->clear_text_buffer();
    get_value_of_interest();
    this->writer->write();
    this->writer->draw_border();
}
