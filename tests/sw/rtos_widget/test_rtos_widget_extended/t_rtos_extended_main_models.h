#pragma once

#include "sw/ui_core/rtos_ui_core.h"
#include "sw/widget/rtos_widget.h"
#include "t_rtos_extended_text_and_graph_widgets.h"
#include "t_rtos_extended_roll_control.h"
#include "t_rtos_extended_config.h"
#include "t_rtos_extended_encoder_controller.h"

#include "utilities/probe/probe.h"


extern my_model my_rtos_model;
extern my_PositionController position_controller;// = my_PositionController(true);
extern my_visu_widget SSD1306_graph_widget;
extern my_text_widget SSD1306_values_widget;
extern my_visu_widget ST7735_graph_widget;
extern my_text_widget ST7735_values_widget;
extern rtos_ST7735 color_display;
extern rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper;
extern my_position_controller_widget SPI_focus_indicator_widget;
extern struct_ConfigTextWidget focus_indicator_config;


void idle_task(void *pxProbe);
void position_controller_task(void *probe);
void my_model_task(void *probe);
void angle_evolution_task(void *probe);
void controlled_position_task(void *position);
void SPI_focus_widget_task(void *probe);