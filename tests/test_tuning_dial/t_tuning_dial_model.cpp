/**
 * @file t_tuning_dial_model.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once
#include "ui_core.h"
#include "widget_text.h"


class FMFrequencyTuning :public ModelText
{
private:
    const float min_frequency = 87.0;
    const float max_frequency = 108.0;
public:
    float current_frequency;
    FMFrequencyTuning(/* args */);
    ~FMFrequencyTuning();
    void process_control_event(UIControlEvent _event);
};

FMFrequencyTuning::FMFrequencyTuning(/* args */) : ModelText(20)
{
}

FMFrequencyTuning::~FMFrequencyTuning()
{
}

void FMFrequencyTuning::process_control_event(UIControlEvent _event)
{
}
