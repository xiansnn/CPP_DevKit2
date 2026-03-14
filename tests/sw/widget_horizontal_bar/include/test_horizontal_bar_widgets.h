#pragma once
// #include "sw/ui_core/ui_core.h"
#include "sw/widget_horizontal_bar/widget_horizontal_bar.h"
#include "test_horizontal_bar_models.h"



// class MySimpleHorizontalBarModel;
// class MyControlledHorizontalBarModel;

class MySimpleHorizontalBarWidget : public WidgetHorizontalBar
{
private:
    /* data */
    void get_value_of_interest();

public:
    MySimpleHorizontalBarWidget(MySimpleHorizontalBarModel *bar_value_model,
                                GraphicDisplayDevice *graphic_display_screen,
                                int max_value, int min_value,
                                struct_ConfigGraphicWidget graph_cfg, CanvasFormat format);
    ~MySimpleHorizontalBarWidget();
};

class MyControlledHorizontalBarWidget : public WidgetHorizontalBar
{
private:
    /* data */
    void get_value_of_interest();

public:
    MyControlledHorizontalBarWidget(MyControlledHorizontalBarModel *bar_value_model,
                                    GraphicDisplayDevice *graphic_display_screen,
                                    int max_value, int min_value,
                                    struct_ConfigGraphicWidget graph_cfg,
                                    CanvasFormat format);
    ~MyControlledHorizontalBarWidget();
};
