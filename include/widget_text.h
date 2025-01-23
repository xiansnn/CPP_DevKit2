/**
 * @file widget_text.h
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
#include "ui_control_event.h"
#include "framebuffer.h"
#include "widget.h"

class ModelText : public UIModelObject
{
private:
uint8_t string_length;
public:
    ModelText(uint string_length_with_status);
    ~ModelText();
};

class WidgetTextWithFocus : public Widget
{
private:

public:
    char convert_status_to_char();
    WidgetTextWithFocus(UIModelObject *text_model,
                        DisplayDevice *display_screen,
                        size_t frame_width,
                        size_t frame_height,
                        uint8_t widget_anchor_x,
                        uint8_t widget_anchor_y,
                        bool widget_with_border,
                        struct_FramebufferText framebuffer_txt_cnf = {.font = font_8x8},
                        uint8_t widget_border_width = 1,
                        FramebufferFormat framebuffer_format = FramebufferFormat::MONO_VLSB);
    ~WidgetTextWithFocus();
    void draw_refresh();
};
