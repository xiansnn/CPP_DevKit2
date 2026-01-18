#include "t_rtos_blinking_led_main_widgets.h"

struct_ConfigTextWidget clock_hour_text_cfg{
    .number_of_column = 2,
    .number_of_line = 1,
    .widget_anchor_x = 6,
    .widget_anchor_y = 128,
    .font = font_16x32,
    .fg_color = ColorIndex::PHOSPHOR,
    .bg_color = ColorIndex::BLACK,
    .wrap = false,
    .widget_with_border = true};

struct_ConfigTextWidget clock_minute_text_cfg{
    .number_of_column = 3,
    .number_of_line = 1,
    .widget_anchor_x = 32 + 6,
    .widget_anchor_y = 128,
    .font = font_16x32,
    .fg_color = ColorIndex::PHOSPHOR,
    .bg_color = ColorIndex::BLACK,
    .wrap = false,
    .widget_with_border = true};

struct_ConfigTextWidget clock_second_text_cfg{
    .number_of_column = 3,
    .number_of_line = 1,
    .widget_anchor_x = 80 + 6,
    .widget_anchor_y = 128 + 8,
    .font = font_12x16,
    .fg_color = ColorIndex::PHOSPHOR,
    .bg_color = ColorIndex::BLACK,
    .wrap = false,
    .widget_with_border = true};

my_hour_text_widget::my_hour_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                                         struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model)
    : rtos_TextWidget(model, text_cfg, format, graphic_display_screen)
{
}

my_hour_text_widget::~my_hour_text_widget()
{
}

void my_hour_text_widget::get_value_of_interest()
{
    this->hour_status = ((myControlledClockTime *)actual_rtos_displayed_model)->get_rtos_status();
    this->hour_value = ((myControlledClockTime *)actual_rtos_displayed_model)->get_value();
}

void my_hour_text_widget::draw()
{
    this->writer->clear_text_buffer();
    get_value_of_interest();

    // prepare to blink 
    if (hour_status == ControlledObjectStatus::IS_ACTIVE)
        this->start_blinking();
    else
        this->stop_blinking();

    // effective draw
    sprintf(this->writer->text_buffer, "%02d", hour_value);
    this->writer->write();
}

void my_hour_text_widget::blink()
{
    //process effective blinking
    this->writer->canvas->fg_color = (blinker->current_blink_phase) ? this->bg_color_backup : this->fg_color_backup;
    this->writer->canvas->bg_color = (blinker->current_blink_phase) ? this->fg_color_backup : this->bg_color_backup;

    if (this->task_handle != nullptr)
        xTaskNotifyGive(this->task_handle);
}

void my_hour_text_widget::save_canvas_color()
{
    this->fg_color_backup = this->writer->canvas->fg_color;
    this->bg_color_backup = this->writer->canvas->bg_color;
}

void my_hour_text_widget::restore_canvas_color()
{
    this->writer->canvas->fg_color = this->fg_color_backup;
    this->writer->canvas->bg_color = this->bg_color_backup;
}

void my_hour_text_widget::show_focus()
{
    this->writer->canvas->fg_color = this->bg_color_backup;
    this->writer->canvas->bg_color = this->fg_color_backup;
}

my_minute_text_widget::my_minute_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                                             struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model)
    : rtos_TextWidget(model, text_cfg, format, graphic_display_screen)
{
}

my_minute_text_widget::~my_minute_text_widget()
{
}

void my_minute_text_widget::get_value_of_interest()
{
    this->minute_status = ((myControlledClockTime *)actual_rtos_displayed_model)->get_rtos_status();
    this->minute_value = ((myControlledClockTime *)actual_rtos_displayed_model)->get_value();
}

void my_minute_text_widget::draw()
{
    this->writer->clear_text_buffer();
    get_value_of_interest();
    if (minute_status == ControlledObjectStatus::IS_ACTIVE)
        this->start_blinking();
    else
        this->stop_blinking();

    // draw
    sprintf(this->writer->text_buffer, ":%02d", minute_value);
    this->writer->write();
}

void my_minute_text_widget::blink()
{
    this->writer->canvas->fg_color = (blinker->current_blink_phase) ? this->bg_color_backup : this->fg_color_backup;
    if (this->task_handle != nullptr)
        xTaskNotifyGive(this->task_handle);
}

void my_minute_text_widget::save_canvas_color()
{
    this->fg_color_backup = this->writer->canvas->fg_color;
    this->bg_color_backup = this->writer->canvas->bg_color;
}

void my_minute_text_widget::restore_canvas_color()
{
    this->writer->canvas->fg_color = this->fg_color_backup;
    this->writer->canvas->bg_color = this->bg_color_backup;
}

void my_minute_text_widget::show_focus()
{
    this->writer->canvas->fg_color = this->bg_color_backup;
    this->writer->canvas->bg_color = this->fg_color_backup;
}

my_second_text_widget::my_second_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                                             struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model)
    : rtos_TextWidget(model, text_cfg, format, graphic_display_screen)
{
}

my_second_text_widget::~my_second_text_widget()
{
}

void my_second_text_widget::get_value_of_interest()
{
    this->second_status = ((myControlledClockTime *)actual_rtos_displayed_model)->get_rtos_status();
    this->second_value = ((myControlledClockTime *)actual_rtos_displayed_model)->get_value();
}

void my_second_text_widget::draw()
{
    this->writer->clear_text_buffer();
    get_value_of_interest();
    if (second_status == ControlledObjectStatus::IS_ACTIVE)
        this->start_blinking();
    else
        this->stop_blinking();
    // draw
    sprintf(this->writer->text_buffer, ":%02d", second_value);
    this->writer->write();
    this->writer->draw_border();
}

void my_second_text_widget::save_canvas_color()
{
    this->fg_color_backup = this->writer->canvas->fg_color;
    this->bg_color_backup = this->writer->canvas->bg_color;
}

void my_second_text_widget::restore_canvas_color()
{
    this->writer->canvas->fg_color = this->fg_color_backup;
    this->writer->canvas->bg_color = this->bg_color_backup;
    this->writer->widget_with_border = false;
}

void my_second_text_widget::blink()
{
    this->writer->widget_with_border = blinker->current_blink_phase;
    if (this->task_handle != nullptr)
        xTaskNotifyGive(this->task_handle);
}

void my_second_text_widget::show_focus()
{
}
