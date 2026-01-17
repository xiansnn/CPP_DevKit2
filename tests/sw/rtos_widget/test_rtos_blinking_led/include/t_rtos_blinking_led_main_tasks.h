#pragma once

#include "t_rtos_blinking_led_config.h"
#include "t_rtos_blinking_led_main_classes.h"
#include "t_rtos_blinking_led_console_widgets.h"
#include "t_rtos_blinking_led_main_widgets.h"
#include "device/ST7735/st7735.h"
#include "device/SSD1306/ssd1306.h"

extern rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper;
extern rtos_ST7735 color_display;

extern myMainClock my_clock;
extern myClockController my_clock_controller;
// extern focus_dummy_widget my_focus_manager_dummy_widget;
extern clock_console_widget my_main_clock_console_widget;

extern rtos_Blinker my_blinker;

extern my_hour_text_widget hour_text_widget;
extern my_minute_text_widget minute_text_widget;
extern my_second_text_widget second_text_widget;

void idle_task(void *pxProbe);
void my_clock_controller_task(void *probe);
void my_clock_main_task(void *probe);
void my_clock_controlled_hour_task(void *probe);
void my_clock_controlled_minute_task(void *probe);
void my_clock_controlled_second_task(void *probe);
void one_second_timer_task(void *probe);

void blinker_task(void* probe);

void clock_controller_console_widget_task(void *probe);
void main_clock_console_widget_task(void *probe);


void SPI_hour_text_widget_task(void *probe);
void SPI_minute_text_widget_task(void *probe);
void SPI_second_text_widget_task(void *probe);
