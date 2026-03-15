#pragma once
#include "sw/ui_core/ui_core.h"

class FMFrequencyTuningModel : public Model
{
private:
    int value{0};
    int min_value;
    int max_value;
    int increment;
    bool is_wrappable;

public:
    FMFrequencyTuningModel(int increment = 1,
                           bool is_wrappable = true,
                           int min_value = 875,
                           int max_value = 1080);
    ~FMFrequencyTuningModel();
    void increment_value();
    void set_clipped_value(int _new_value);
    int get_min_value();
    int get_value();

};

class FMVolumeModel : public Model
{
private:
    int value{0};
    int min_value;
    int max_value;
    int increment;
    bool is_wrappable;

public:
    FMVolumeModel(int increment = 1,
                  bool is_wrappable = true,
                  int min_value = -20,
                  int max_value = +3);
    ~FMVolumeModel();
    void increment_value();
    void set_clipped_value(int _new_value);
    int get_min_value();
    int get_value();

};
