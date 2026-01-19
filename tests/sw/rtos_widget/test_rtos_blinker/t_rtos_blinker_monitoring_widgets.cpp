#include "t_rtos_blinker_monitoring_widgets.h"
#include "t_rtos_blinker_main_classes.h"

extern std::map<ControlledObjectStatus, std::string> status_to_string;

struct_ConfigTextWidget clock_monitoring_text_cfg{
    .number_of_column = 16,
    .number_of_line = 8,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .font = font_8x8,
    .fg_color = ColorIndex::WHITE,
    .bg_color = ColorIndex::BLACK,
    .wrap = true,
    .widget_with_border = false};

struct_ConfigTextWidget controller_monitoring_text_cfg{
    .number_of_column = 16,
    .number_of_line = 4,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .font = font_8x8,
    .fg_color = ColorIndex::WHITE,
    .bg_color = ColorIndex::BLACK,
    .wrap = true,
    .widget_with_border = false};

my_controller_monitoring_widget::my_controller_monitoring_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                                                                 struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model)
    : rtos_TextWidget(model, text_cfg, format, graphic_display_screen)
{
}

my_controller_monitoring_widget::~my_controller_monitoring_widget()
{
}

void my_controller_monitoring_widget::get_value_of_interest()
{
    myClockController *clock_controller = (myClockController *)actual_rtos_displayed_model;
    controller_status = status_to_string[clock_controller->get_rtos_status()];
    focus_index = clock_controller->get_current_focus_index();
    focus_name = ((myControlledClockTime *)clock_controller->managed_rtos_models[focus_index])->name;
}

void my_controller_monitoring_widget::draw()
{
    this->writer->clear_text_buffer();
    get_value_of_interest();
    // draw
    sprintf(this->writer->text_buffer, "CTR[%s]\n\nFocus(%s)", controller_status.c_str(), focus_name.c_str());
    this->writer->write();
    this->writer->draw_border();
}

my_clock_monitoring_widget::my_clock_monitoring_widget(rtos_GraphicDisplayDevice *graphic_display_screen, struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model)
    : rtos_TextWidget(model, text_cfg, format, graphic_display_screen)
{
}

my_clock_monitoring_widget::~my_clock_monitoring_widget()
{
}

void my_clock_monitoring_widget::get_value_of_interest()
{
    myMainClock *main_clock = (myMainClock *)actual_rtos_displayed_model;
    myControlledClockTime clock_hh = main_clock->hour;
    myControlledClockTime clock_mm = main_clock->minute;
    myControlledClockTime clock_ss = main_clock->second;

    clock_status = status_to_string[main_clock->get_rtos_status()];
    hour_status = status_to_string[clock_hh.get_rtos_status()];
    minute_status = status_to_string[clock_mm.get_rtos_status()];
    second_status = status_to_string[clock_ss.get_rtos_status()];

    hh = clock_hh.get_value();
    mm = clock_mm.get_value();
    ss = clock_ss.get_value();
}

void my_clock_monitoring_widget::draw()
{
    this->writer->clear_text_buffer();
    get_value_of_interest();
    // draw
    sprintf(this->writer->text_buffer, "CLK[%s]\n\nHH[%s]\nMM[%s]\nSS[%s]\n\n<%02d:%02d:%02d>",
            clock_status.c_str(), hour_status.c_str(), minute_status.c_str(), second_status.c_str(), hh, mm, ss);
    this->writer->write();
    this->writer->draw_border();
}
