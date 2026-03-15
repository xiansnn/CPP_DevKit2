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

 #include "t_tuning_dial_models.h"


FMFrequencyTuningModel::FMFrequencyTuningModel(int increment,
                                               bool is_wrappable,
                                               int min_value,
                                               int max_value)
: Model()
{
    this->min_value = min_value;
    this->max_value = max_value;
    this->increment = increment;
    this->is_wrappable = is_wrappable;
}

FMFrequencyTuningModel::~FMFrequencyTuningModel()
{
}

void FMFrequencyTuningModel::increment_value()
{
    int previous_value = value;
    value += increment;
    if (value > max_value)
        value = (is_wrappable) ? min_value : max_value;
    set_change_flag();
}

void FMFrequencyTuningModel::set_clipped_value(int _new_value)
{
    int previous_value = value;
    value = std::min(max_value, std::max(min_value, _new_value));
}

int FMFrequencyTuningModel::get_min_value()
{
    return this->min_value;
}

int FMFrequencyTuningModel::get_value()
{
    return this->value;
}

FMVolumeModel::FMVolumeModel(int increment,
                             bool is_wrappable,
                             int min_value,
                             int max_value)
: Model()
{
    this->min_value = min_value;
    this->max_value = max_value;
    this->increment = increment;
    this->is_wrappable = is_wrappable;
}

FMVolumeModel::~FMVolumeModel()
{
}

void FMVolumeModel::increment_value()
{
    int previous_value = value;
    value += increment;
    if (value > max_value)
        value = (is_wrappable) ? min_value : max_value;
    set_change_flag();
}

void FMVolumeModel::set_clipped_value(int _new_value)
{
    int previous_value = value;
    value = std::min(max_value, std::max(min_value, _new_value));
}

int FMVolumeModel::get_min_value()
{
    return this->min_value;
}

int FMVolumeModel::get_value()
{
    return this->value;
}
