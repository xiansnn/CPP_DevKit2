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

#include "t_tuning_dial_widgets.h"



FMFrequencyWidget::FMFrequencyWidget(GraphicDisplayDevice *graphic_display_screen,
                                     struct_ConfigTextWidget text_cfg, CanvasFormat format,
                                     Model *displayed_object)
    : TextWidget(graphic_display_screen, text_cfg, format, displayed_object)
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
                               struct_ConfigTextWidget text_cfg, CanvasFormat format,
                               Model *displayed_object)
    : TextWidget(graphic_display_screen, text_cfg, format, displayed_object)
{
}

FMVolumeWidget::~FMVolumeWidget()
{
}

void FMVolumeWidget::get_value_of_interest()
{
    sprintf(this->text_buffer, "%*d dB\n", 3, ((FMVolumeModel *)this->actual_displayed_model)->get_value());
}
