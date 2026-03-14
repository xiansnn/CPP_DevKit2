/**
 * @file test_horizontal_bar.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "test_horizontal_bar_widgets.h"


MySimpleHorizontalBarWidget::MySimpleHorizontalBarWidget(MySimpleHorizontalBarModel *bar_value_model,
                                                         GraphicDisplayDevice *graphic_display_screen,
                                                         int max_value, int min_value,
                                                         struct_ConfigGraphicWidget graph_cfg, CanvasFormat format)
    : WidgetHorizontalBar(bar_value_model,
                          graphic_display_screen,
                          max_value, min_value,
                          graph_cfg, format)
{
}

MySimpleHorizontalBarWidget::~MySimpleHorizontalBarWidget()
{
}

void MySimpleHorizontalBarWidget::get_value_of_interest()
{
    set_level(((MySimpleHorizontalBarModel *)actual_displayed_model)->get_value());
}


MyControlledHorizontalBarWidget::MyControlledHorizontalBarWidget(MyControlledHorizontalBarModel *bar_value_model,
                                                                 GraphicDisplayDevice *graphic_display_screen,
                                                                 int max_value, int min_value,
                                                                 struct_ConfigGraphicWidget graph_cfg,
                                                                 CanvasFormat format)
    : WidgetHorizontalBar(bar_value_model,
                          graphic_display_screen,
                          max_value, min_value,
                          graph_cfg, format)
{
}

MyControlledHorizontalBarWidget::~MyControlledHorizontalBarWidget()
{
}

void MyControlledHorizontalBarWidget::get_value_of_interest()
{
    set_level(((MyControlledHorizontalBarModel *)actual_displayed_model)->get_value());
}

