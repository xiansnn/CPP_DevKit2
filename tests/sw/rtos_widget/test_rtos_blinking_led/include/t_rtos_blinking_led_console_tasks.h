#pragma once

#include "t_rtos_blinking_led_config.h"
#include "t_rtos_blinking_led_console_widgets.h"

extern focus_console_widget my_focus_manager_console_widget;
extern clock_console_widget my_main_clock_console_widget;

void clock_controller_console_widget_task(void *probe);
void main_clock_console_widget_task(void *probe);

