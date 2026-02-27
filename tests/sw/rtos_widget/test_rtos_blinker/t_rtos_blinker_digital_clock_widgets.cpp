#include "t_rtos_blinker_digital_clock_widgets.h"

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


my_digital_clock_widget::my_digital_clock_widget(ClockElementType clock_element_type, rtos_GraphicDisplayDevice *graphic_display_screen,
                                                 rtos_Blinker *blinker, struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model)
    : rtos_TextWidget(model, text_cfg, format, graphic_display_screen), rtos_BlinkingWidget(blinker)
{
    this->clock_element_type = clock_element_type;
    save_canvas_color();
}

my_digital_clock_widget::~my_digital_clock_widget()
{
}

void my_digital_clock_widget::get_value_of_interest()
{ 
    this->digital_clock_status = ((myControlledClockTime *)actual_rtos_displayed_model)->get_rtos_status();
    this->digital_clock_value = ((myControlledClockTime *)actual_rtos_displayed_model)->get_value();
}

void my_digital_clock_widget::draw()
{
    this->writer->clear_text_buffer();
    get_value_of_interest();

    // prepare to blink
    convert_status_to_blinking_behavior(digital_clock_status);
    // effective draw
    switch (clock_element_type)
    {
    case ClockElementType::HOUR:
        sprintf(this->writer->text_buffer, "%02d", digital_clock_value);
        break;
   case ClockElementType::MINUTE:
        sprintf(this->writer->text_buffer, ":%02d", digital_clock_value);
        break;
   case ClockElementType::SECOND:
        sprintf(this->writer->text_buffer, ":%02d", digital_clock_value);
        break;
    default:
        break;
    }
    this->writer->write();

}

void my_digital_clock_widget::blink()
{
    // process effective blinking
    this->writer->canvas->fg_color = (blinker->current_blink_phase) ? this->bg_color_backup : this->fg_color_backup;
    this->writer->canvas->bg_color = (blinker->current_blink_phase) ? this->fg_color_backup : this->bg_color_backup;

    if (this->task_handle != nullptr)
        xTaskNotifyGive(this->task_handle);
}

void my_digital_clock_widget::save_canvas_color()
{
    this->fg_color_backup = this->writer->canvas->fg_color;
    this->bg_color_backup = this->writer->canvas->bg_color;
}

void my_digital_clock_widget::restore_canvas_color()
{
    this->writer->canvas->fg_color = this->fg_color_backup;
    this->writer->canvas->bg_color = this->bg_color_backup;
}

void my_digital_clock_widget::set_focus_color()
{
    this->writer->canvas->fg_color = this->bg_color_backup;
    this->writer->canvas->bg_color = this->fg_color_backup;
}
