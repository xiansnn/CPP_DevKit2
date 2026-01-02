#pragma once

#include "t_rtos_blinking_led_config.h"
#include "t_rtos_blinking_led_main_classes.h"


// #include "t_rtos_extended_text_and_graph_widgets.h"
// #include "t_rtos_extended_roll_control.h"

// extern struct_ConfigTextWidget focus_indicator_config;


// extern my_visu_widget SSD1306_graph_widget;
// extern my_text_widget SSD1306_values_widget;
// extern my_visu_widget ST7735_graph_widget;
// extern my_text_widget ST7735_values_widget;
// extern my_position_controller_widget SPI_focus_indicator_widget;

// extern rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper;
// extern rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper;

// extern rtos_ST7735 color_display;
// extern rtos_SSD1306 left_display;
// extern rtos_SSD1306 right_display;

extern myMainClock my_clock;
extern myFocusManager focus_led_manager;

void idle_task(void *pxProbe);
void focus_led_manager_task(void *probe);
void my_main_clock_task(void *probe);
void one_second_timer_task(void *probe);
// void controlled_position_task(void *position);

// void SPI_focus_widget_task(void *probe);
// void SPI_values_widget_task(void *probe);
// void SPI_graph_widget_task(void *probe);
// void I2C_right_graph_widget_task(void *probe);
// void I2C_left_values_widget_task(void *probe);