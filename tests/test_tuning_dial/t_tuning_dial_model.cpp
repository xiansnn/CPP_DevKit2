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

class FMFrequencyTuningModel : public UIControlledIncrementalValue
{
private:
public:
    FMFrequencyTuningModel(int increment = 1,
                           bool is_wrappable = true,
                           int min_value = 875,
                           int max_value = 1080);
    ~FMFrequencyTuningModel();
    void process_control_event(UIControlEvent _event = UIControlEvent::NONE);
};

class FMVolumeModel : public UIControlledIncrementalValue
{
private:
public:
    FMVolumeModel(int increment = 1,
                  bool is_wrappable = true,
                  int min_value = -20,
                  int max_value = +3);
    ~FMVolumeModel();
    void process_control_event(UIControlEvent _event = UIControlEvent::NONE);
};

FMFrequencyTuningModel::FMFrequencyTuningModel(int increment,
                                               bool is_wrappable,
                                               int min_value,
                                               int max_value)
    : UIControlledIncrementalValue(min_value, max_value, is_wrappable, increment)
{
}

FMFrequencyTuningModel::~FMFrequencyTuningModel()
{
}

void FMFrequencyTuningModel::process_control_event(UIControlEvent _event)
{
    this->increment_value();
}

FMVolumeModel::FMVolumeModel(int increment,
                             bool is_wrappable,
                             int min_value,
                             int max_value)
    : UIControlledIncrementalValue(min_value, max_value, is_wrappable, increment)
{
}

FMVolumeModel::~FMVolumeModel()
{
}

void FMVolumeModel::process_control_event(UIControlEvent _event)
{
    this->increment_value();
}
