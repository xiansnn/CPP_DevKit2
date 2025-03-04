/**
 * @file widget_focus_indicator.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "widget_focus_indicator.h"

WidgetFocusIndicator::WidgetFocusIndicator(Model *actual_displayed_model,
                                           GraphicDisplayDevice *display_screen,
                                           struct_ConfigGraphicWidget graph_cfg)
    : WidgetBlinkingSquareLed(actual_displayed_model, display_screen, graph_cfg)
{
}

WidgetFocusIndicator::~WidgetFocusIndicator()
{
}