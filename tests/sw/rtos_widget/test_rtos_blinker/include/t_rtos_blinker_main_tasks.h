#pragma once

#include "t_rtos_blinker_config.h"

#include "t_rtos_blinker_main_classes.h"

#include "device/ST7735/st7735.h"
#include "device/SSD1306/ssd1306.h"

#include "t_rtos_blinker_console_widgets.h"
#include "t_rtos_blinker_main_widgets.h"
#include "t_rtos_blinker_clock_widget.h"

extern rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper;
extern rtos_ST7735 color_display;

extern myMainClock my_clock;
extern myClockController my_clock_controller;
extern clock_console_widget my_main_clock_console_widget;

extern rtos_Blinker my_blinker;

extern my_hour_text_widget hour_text_widget;
extern my_minute_text_widget minute_text_widget;
extern my_second_text_widget second_text_widget;

extern ClockWidget clock_widget;
extern ClockWidgetElement clock_hour_widget_element;
extern ClockWidgetElement clock_minute_widget_element;
extern ClockWidgetElement clock_second_widget_element;

void idle_task(void *pxProbe);
void one_second_timer_task(void *probe);
void my_clock_main_task(void *probe);
void my_clock_controller_task(void *probe);
void my_clock_controlled_hour_task(void *probe);
void my_clock_controlled_minute_task(void *probe);
void my_clock_controlled_second_task(void *probe);

void clock_controller_console_widget_task(void *probe);
void main_clock_console_widget_task(void *probe);

void clock_widget_task(void *probe);

void blinker_task(void *probe);

void SPI_hour_text_widget_task(void *probe);
void SPI_minute_text_widget_task(void *probe);
void SPI_second_text_widget_task(void *probe);

void SPI_clock_hour_widget_element_task(void *probe);
void SPI_clock_minute_widget_element_task(void *probe);
void SPI_clock_second_widget_element_task(void *probe);
