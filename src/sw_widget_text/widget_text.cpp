/**
 * @file widget_text.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "widget_text.h"

char WidgetTextWithFocus::convert_status_to_char()
{
    char status_char;
    switch (this->actual_displayed_model->get_status())
    {
    case ControlledObjectStatus::HAS_FOCUS:
        status_char = '>';
        break;
    case ControlledObjectStatus::IS_ACTIVE:
        status_char = '\xB2';
        break;
    default:
        status_char = ' ';
        break;
    }
    return status_char;
}

WidgetTextWithFocus::WidgetTextWithFocus(UIModelObject *text_model,
                                         DisplayDevice *display_screen,
                                         size_t frame_width,
                                         size_t frame_height,
                                         uint8_t widget_anchor_x,
                                         uint8_t widget_anchor_y,
                                         bool widget_with_border,
                                         struct_FramebufferText framebuffer_txt_cnf,
                                         uint8_t widget_border_width,
                                         FramebufferFormat framebuffer_format)
    : Widget(display_screen, frame_width, frame_height, widget_anchor_x, widget_anchor_y, widget_with_border, widget_border_width, framebuffer_format, framebuffer_txt_cnf)
{
    this->actual_displayed_model = text_model;
}

WidgetTextWithFocus::~WidgetTextWithFocus()
{
}

void WidgetTextWithFocus::draw_refresh()
{
}

ModelText::ModelText(uint string_length_with_status)
{
    this->string_length = string_length_with_status;
}

ModelText::~ModelText()
{
}
