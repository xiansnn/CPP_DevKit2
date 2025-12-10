#pragma once


#include <pico/stdio.h>
#include <math.h>
#include <numbers>
#include <sstream>
#include <iomanip>
#include <string>

#include "sw/widget/rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"
#include "device/ST7735/st7735.h"

class my_model : public rtos_Model
{
private:
    /* data */
public:
    my_model();
    ~my_model();
    int roll{0};
    int pitch{0};
    int cycle{0};
    void update_cycle(int i, int sign);
};


class my_text_widget : public rtos_TextWidget
{

public:
    my_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model = nullptr);
    my_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigTextWidget text_cfg, CanvasFormat format,
                   uint8_t x, uint8_t y, rtos_Model *model = nullptr);
    ~my_text_widget();
    void get_value_of_interest();
    void draw();
};



class my_visu_widget : public rtos_GraphicWidget
{
private:
    int roll{0};
    int pitch{0};

public:
    my_visu_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                   struct_ConfigGraphicWidget graph_cfg, CanvasFormat format, rtos_Model *model);
    ~my_visu_widget();
    void get_value_of_interest();
    void draw();
};
