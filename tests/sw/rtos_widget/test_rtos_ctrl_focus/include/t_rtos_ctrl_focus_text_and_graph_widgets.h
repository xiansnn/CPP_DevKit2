/**
 * @file t_rtos_extended_text_and_graph_widgets.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-12-27
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <pico/stdio.h>
#include <math.h>

#include "sw/widget/rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"

extern struct_ConfigGraphicWidget ST7735_graph_config;
extern struct_ConfigTextWidget SSD1306_title_config;
extern struct_ConfigTextWidget SSD1306_values_config;
extern struct_ConfigGraphicWidget SSD1306_graph_config;

extern struct_ConfigTextWidget ST7735_angle_config;
extern struct_ConfigTextWidget ST7735_H_position_config;
extern struct_ConfigTextWidget ST7735_V_position_config;

class my_angle_widget : public rtos_TextWidget, public rtos_BlinkingWidget
{
private:
    int value_angle{0};
    ControlledObjectStatus status;

public:
    my_angle_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                    struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model = nullptr);
    ~my_angle_widget();
    void get_value_of_interest();
    void draw();
    void save_canvas_color() override;
    void restore_canvas_color() override;
    void blink() override;
    void show_focus() override;
};
class my_H_position_widget : public rtos_TextWidget, public rtos_BlinkingWidget
{
private:
    int value_H_pos{0};
    ControlledObjectStatus status;

public:
    my_H_position_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                         struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model = nullptr);
    ~my_H_position_widget();
    void get_value_of_interest();
    void draw();
    void save_canvas_color() override;
    void restore_canvas_color() override;
    void blink() override;
    void show_focus() override;
};
class my_V_position_widget : public rtos_TextWidget, public rtos_BlinkingWidget
{
private:
    int value_V_pos{0};
    ControlledObjectStatus status;

public:
    my_V_position_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                         struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model = nullptr);
    ~my_V_position_widget();
    void get_value_of_interest();
    void draw();
    void save_canvas_color() override;
    void restore_canvas_color() override;
    void blink() override;
    void show_focus() override;
};

/// @brief Construct an implementation of TextWidget for test_ui_core program.
class my_text_widget : public rtos_TextWidget
{
private:
    int value_angle{0};
    int value_x_pos{0};
    int value_y_pos{0};

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

/// @brief Construct an implementation of GraphicWidget for test_ui_core program.
class my_graphic_widget : public rtos_GraphicWidget
{
private:
    int graph_angle{0};
    int graph_vertical_pos{0};
    int graph_horizontal_pos{0};

public:
    my_graphic_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                      struct_ConfigGraphicWidget graph_cfg, CanvasFormat format, rtos_Model *model);
    ~my_graphic_widget();
    void get_value_of_interest();
    void draw();
};
