#pragma once

#include "sw/widget/widget.h"
#include "t_tuning_dial_models.h"



class FMVolumeWidget : public TextWidget
{
private:
    /* data */
public:
    FMVolumeWidget(GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg, CanvasFormat format,
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
                      struct_ConfigTextWidget text_cfg, CanvasFormat format,
                      Model *displayed_object);
    ~FMFrequencyWidget();
    void get_value_of_interest();
};
