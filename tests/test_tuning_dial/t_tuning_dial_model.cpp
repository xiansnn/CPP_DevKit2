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

class FMFrequencyTuning : public UIModelObject
{
private:
    const float min_frequency = 87.0;
    const float max_frequency = 108.0;
    float current_frequency;

public:
    void set_current_frequency(float _frequency);
    float get_current_frequency();
    void increment_frequency(float _increment_value);
    void decrement_frequency(float _decrement_value);
    FMFrequencyTuning(/* args */);
    ~FMFrequencyTuning();
    void process_control_event(UIControlEvent _event = UIControlEvent::NONE);
};

void FMFrequencyTuning::set_current_frequency(float _frequency)
{
    float previous_value = current_frequency;
    current_frequency = std::min(max_frequency, std::max(min_frequency, _frequency));
    if ((current_frequency - previous_value) > 0.5)
        set_change_flag();
}

float FMFrequencyTuning::get_current_frequency()
{
    return this->current_frequency;
}

void FMFrequencyTuning::increment_frequency(float _increment_value)
{
    this->current_frequency += _increment_value;
}

void FMFrequencyTuning::decrement_frequency(float _decrement_value)
{
    this->current_frequency -= _decrement_value;
}

FMFrequencyTuning::FMFrequencyTuning(/* args */) : UIModelObject()
{
}

FMFrequencyTuning::~FMFrequencyTuning()
{
}

void FMFrequencyTuning::process_control_event(UIControlEvent _event)
{
    this->set_change_flag();
}
