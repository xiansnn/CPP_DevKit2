/**
 * @file t_rtos_extended_roll_control.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-12-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include "sw/ui_core/rtos_ui_core.h"
#include "t_rtos_extended_config.h"
#include "t_rtos_extended_text_and_graph_widgets.h"


class my_model;

/// @brief Construct an implementation of UIControlledIncrementalValue for test_ui_core program.
class my_ControlledCenterPosition : public rtos_UIControlledModel, public core_IncrementControlledModel
{
private:
public:
    my_model *parent_model;
    std::string name;
    my_ControlledCenterPosition(std::string name, my_model *controlled_model,
                              int min_value = 0, int max_value = 10, bool is_wrappable = false, int increment = 1);
    ~my_ControlledCenterPosition();
    void process_control_event(struct_ControlEventData control_event);
};


class my_ControlledAnglePosition : public rtos_UIControlledModel, public core_CircularIncremetalControlledModel
{
private:
public:
    my_model *parent_model;
    std::string name;
    my_ControlledAnglePosition(std::string name, my_model *controlled_model,
                              int min_value = 0, int max_value = 360, int increment = 1);
    ~my_ControlledAnglePosition();
    void process_control_event(struct_ControlEventData control_event);
};

/// @brief Construct an implementation of UIModelManager for test_ui_core program.
class my_model : public rtos_UIControlledModel
{
private:
    /* data */
public:
    my_model();
    ~my_model();
    my_ControlledCenterPosition angle;
    my_ControlledCenterPosition x_pos;
    my_ControlledCenterPosition y_pos;
    void process_control_event(struct_ControlEventData control_event);
};

/// @brief Construct an implementation of UIModelManager for test_ui_core program.
class my_PositionController : public rtos_UIModelManager
{
private:
public:
    my_PositionController(bool is_wrapable = false);
    ~my_PositionController();
    void process_control_event(struct_ControlEventData control_event);
};

/// @brief Construct an implementation of TextWidget for test_ui_core program.
class my_position_controller_widget : public rtos_TextWidget
{
private:
    std::string focus_on_value_name;
    ControlledObjectStatus manager_status;

public:
    my_position_controller_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                                  struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_UIModelManager *manager);
    ~my_position_controller_widget();
    void get_value_of_interest();
    void draw();
};