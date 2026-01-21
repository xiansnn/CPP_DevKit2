/**
 * @file t_rtos_extended_text_and_graph_widgets.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-12-27
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "t_rtos_ctrl_focus_config.h"
#include "t_rtos_ctrl_focus_main_model_classes.h"
#include "t_rtos_ctrl_focus_text_and_graph_widgets.h"

struct_ConfigTextWidget SSD1306_title_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .font = font_12x16,
    .auto_next_char = true};

struct_ConfigTextWidget SSD1306_values_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 68,
    .widget_anchor_y = 0,
    .font = font_12x16};

struct_ConfigGraphicWidget SSD1306_graph_config{
    .canvas_width_pixel = 128,
    .canvas_height_pixel = 56,
    .canvas_foreground_color = ColorIndex::WHITE,
    .canvas_background_color = ColorIndex::BLACK,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};

struct_ConfigTextWidget ST7735_title_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 0,
    .widget_anchor_y = 64,
    .font = font_12x16,
    .auto_next_char = true};

struct_ConfigTextWidget ST7735_values_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 68,
    .widget_anchor_y = 64,
    .font = font_12x16};

struct_ConfigGraphicWidget ST7735_graph_config{
    .canvas_width_pixel = 128,
    .canvas_height_pixel = 56,
    .canvas_foreground_color = ColorIndex::YELLOW,
    .canvas_background_color = ColorIndex::RED,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};

my_text_widget::my_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model)
    : rtos_TextWidget(model, text_cfg, format, graphic_display_screen)

{
}

my_text_widget::my_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg, CanvasFormat format,
                               uint8_t x, uint8_t y, rtos_Model *model)
    : rtos_TextWidget(model, text_cfg, format, x, y, graphic_display_screen) {}

my_text_widget::~my_text_widget() {}

void my_text_widget::get_value_of_interest()
{
    this->value_angle = ((my_model *)this->actual_rtos_displayed_model)->angle.get_value();
    this->value_x_pos = ((my_model *)this->actual_rtos_displayed_model)->x_pos.get_value();
    this->value_y_pos = ((my_model *)this->actual_rtos_displayed_model)->y_pos.get_value();
}

my_visu_widget::my_visu_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigGraphicWidget graph_cfg, CanvasFormat format, rtos_Model *model)
    : rtos_GraphicWidget(model, graph_cfg, format, graphic_display_screen) {}

my_visu_widget::~my_visu_widget() {}

void my_visu_widget::get_value_of_interest()
{
    this->graph_angle = ((my_model *)this->actual_rtos_displayed_model)->angle.get_value();
    this->graph_vertical_pos = ((my_model *)this->actual_rtos_displayed_model)->y_pos.get_value();
    this->graph_horizontal_pos = ((my_model *)this->actual_rtos_displayed_model)->x_pos.get_value();
}

void my_visu_widget::draw()
{
    drawer->clear_widget();
    get_value_of_interest();

    // compute cercle center
    float widget_horizontal_center = drawer->canvas->canvas_width_pixel / 2;
    float widget_vertical_center = drawer->canvas->canvas_height_pixel / 2;
    float radius = widget_vertical_center - 2 * drawer->widget_border_width; // radius -2 to fit inside the rectangle
    float circle_vertical_center = widget_vertical_center - graph_vertical_pos;
    float circle_horizontal_center = widget_horizontal_center + graph_horizontal_pos;
    // compute radius line
    float sin_angle = sin(std::numbers::pi / 180.0 * graph_angle);
    float cos_angle = cos(std::numbers::pi / 180.0 * graph_angle);
    int x0 = circle_horizontal_center;
    int y0 = circle_vertical_center;
    int x1 = circle_horizontal_center + radius * cos_angle;
    int y1 = circle_vertical_center - radius * sin_angle;

    this->drawer->canvas->fill_canvas_with_color(drawer->canvas->bg_color);

    this->drawer->circle(radius, circle_horizontal_center, circle_vertical_center, false, ColorIndex::WHITE);
    this->drawer->line(x0, y0, x1, y1, ColorIndex::WHITE);

    drawer->draw_border(drawer->canvas->fg_color);
}

void my_text_widget::draw()
{
    this->writer->clear_text_buffer();
    get_value_of_interest();
    // draw
    sprintf(this->writer->text_buffer, "%4d\xF8%+4d\n%+4d", value_angle, value_x_pos, value_y_pos);
    this->writer->write();
    this->writer->draw_border();
}
// void my_text_widget::save_canvas_color()
// {
//     this->fg_color_backup = this->writer->canvas->fg_color;
//     this->bg_color_backup = this->writer->canvas->bg_color;
// }

// void my_text_widget::restore_canvas_color()
// {
//     this->writer->canvas->fg_color = this->fg_color_backup;
//     this->writer->canvas->bg_color = this->bg_color_backup;
// }

// void my_text_widget::blink()
// {
//     //process effective blinking
//     this->writer->canvas->fg_color = (blinker->current_blink_phase) ? this->bg_color_backup : this->fg_color_backup;
//     this->writer->canvas->bg_color = (blinker->current_blink_phase) ? this->fg_color_backup : this->bg_color_backup;

//     if (this->task_handle != nullptr)
//         xTaskNotifyGive(this->task_handle);

// }

// void my_text_widget::show_focus()
// {
//     this->writer->canvas->fg_color = this->bg_color_backup;
//     this->writer->canvas->bg_color = this->fg_color_backup;

// }
