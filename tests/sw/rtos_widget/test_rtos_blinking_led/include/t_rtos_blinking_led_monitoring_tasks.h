#pragma once

#include "t_rtos_blinking_led_config.h"
#include "t_rtos_blinking_led_monitoring_widgets.h"
#include "device/SSD1306/ssd1306.h"

extern rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper;
extern rtos_SSD1306 left_display;
extern rtos_SSD1306 right_display;
extern my_clock_monitoring_widget clock_monitoring_widget;
extern my_controller_monitoring_widget controller_monitoring_widget;

void controller_monitoring_widget_task(void *probe);
void clock_monitoring_widget_task(void *probe);

