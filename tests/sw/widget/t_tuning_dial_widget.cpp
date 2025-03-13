/**
 * @file t_tuning_dial_widget.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-03-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "sw/ui_core.h"
#include "sw/widget.h"
#include "t_tuning_dial_model.cpp"
class FMVolumeWidget : public TextWidget
{
private:
    /* data */
public:
    FMVolumeWidget(GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg,
                   Model *displayed_object);
    ~FMVolumeWidget();

    void get_value_of_interest();
};

class FMFrequencyWidget : public TextWidget
{
private:
    /* data */
public:
    FMFrequencyWidget(GraphicDisplayDevice *graphic_display_screen,
                      struct_ConfigTextWidget text_cfg,
                      Model *displayed_object);
    ~FMFrequencyWidget();
    void get_value_of_interest();
};

FMFrequencyWidget::FMFrequencyWidget(GraphicDisplayDevice *graphic_display_screen,
                                     struct_ConfigTextWidget text_cfg,
                                     Model *displayed_object)
    : TextWidget(graphic_display_screen, text_cfg, displayed_object)
{
}

FMFrequencyWidget::~FMFrequencyWidget()
{
}

void FMFrequencyWidget::get_value_of_interest()
{
    sprintf(this->text_buffer, "%5.1f MHz\n", ((FMFrequencyTuningModel *)this->actual_displayed_model)->get_value() / 10.0);
}

FMVolumeWidget::FMVolumeWidget(GraphicDisplayDevice *graphic_display_screen,
                               struct_ConfigTextWidget text_cfg,
                               Model *displayed_object)
    : TextWidget(graphic_display_screen, text_cfg, displayed_object)
{
}

FMVolumeWidget::~FMVolumeWidget()
{
}

void FMVolumeWidget::get_value_of_interest()
{
    sprintf(this->text_buffer, "%*d dB\n", 3, ((FMVolumeModel *)this->actual_displayed_model)->get_value());
}
