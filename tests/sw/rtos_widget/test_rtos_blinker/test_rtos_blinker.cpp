#include "t_rtos_blinker_config.h"
#include "t_rtos_blinker_display_setup.h"
#include "t_rtos_blinker_encoder_controller.h"
#include "t_rtos_blinker_main_classes.h"
#include "t_rtos_blinker_main_tasks.h"
#include "t_rtos_blinker_main_widgets.h"
#include "t_rtos_blinker_monitoring_tasks.h"
#include "t_rtos_blinker_monitoring_widgets.h"
#include "t_rtos_blinker_console_tasks.h"
#include "t_rtos_blinker_console_widgets.h"
#include "t_rtos_blinker_clock_widget.h"

// ##### Configuration structures #####
Probe p0 = Probe(0);
Probe p1 = Probe(1);

#if !defined(SHOW_MONITORING_WIDGET)
Probe p2 = Probe(2); //used by I2C
Probe p3 = Probe(3); //used by I2C
#endif 

Probe p4 = Probe(4);
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

// ##### main classes #####
myClockController my_clock_controller = myClockController(true);
myMainClock my_clock = myMainClock();
rtos_Blinker my_blinker = rtos_Blinker(BLINKING_PERIOD_ms/2);

// ##### ST7735 setup #####
rtos_HW_SPI_Master spi_master = rtos_HW_SPI_Master(cfg_spi,
                                                   DMA_IRQ_0, end_of_TX_DMA_xfer_handler);
rtos_ST7735 color_display = rtos_ST7735(&spi_master, cfg_st7735);
rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper = rtos_GraphicDisplayGateKeeper();

// ##### SSD1306 setup #####
#ifdef SHOW_MONITORING_WIDGET
rtos_HW_I2C_Master i2c_master = rtos_HW_I2C_Master(cfg_i2c);
rtos_SSD1306 left_display = rtos_SSD1306(&i2c_master, cfg_left_screen);
rtos_SSD1306 right_display = rtos_SSD1306(&i2c_master, cfg_right_screen);
rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper = rtos_GraphicDisplayGateKeeper();
#endif

// ##### KY040 encoder controller setup #####
rtos_SwitchButton central_switch = rtos_SwitchButton(GPIO_CENTRAL_SWITCH,
                                                     &ky040_encoder_irq_call_back, my_clock_controller.control_event_input_queue,
                                                     cfg_central_switch);
rtos_RotaryEncoder encoder = rtos_RotaryEncoder(GPIO_ENCODER_CLK, GPIO_ENCODER_DT,
                                                &ky040_encoder_irq_call_back, my_clock_controller.control_event_input_queue,
                                                cfg_encoder_clk);

// ##### Widgets #####
#ifdef SHOW_CONSOLE_WIDGET
focus_console_widget my_focus_manager_console_widget = focus_console_widget(&my_clock_controller, nullptr);
clock_console_widget my_main_clock_console_widget = clock_console_widget(&my_clock, nullptr);
#endif
#ifdef SHOW_MONITORING_WIDGET
my_controller_monitoring_widget controller_monitoring_widget = my_controller_monitoring_widget(&right_display, controller_monitoring_text_cfg, CanvasFormat::MONO_VLSB, &my_clock_controller);
my_clock_monitoring_widget clock_monitoring_widget = my_clock_monitoring_widget(&left_display, clock_monitoring_text_cfg, CanvasFormat::MONO_VLSB, &my_clock);
#endif
my_hour_text_widget hour_text_widget = my_hour_text_widget(&color_display, clock_hour_text_cfg, CanvasFormat::RGB565_16b, &my_clock.hour);
my_minute_text_widget minute_text_widget = my_minute_text_widget(&color_display, clock_minute_text_cfg, CanvasFormat::RGB565_16b, &my_clock.minute);
my_second_text_widget second_text_widget = my_second_text_widget(&color_display, clock_second_text_cfg, CanvasFormat::RGB565_16b, &my_clock.second);

ClockWidget clock_widget = ClockWidget(&my_clock, clock_widget_config, CanvasFormat::RGB565_16b, &color_display);


HourWidgetElement clock_hour_widget_element = HourWidgetElement(&clock_widget, &my_clock.hour, hour_widget_element_config);
MinuteWidgetElement clock_minute_widget_element = MinuteWidgetElement(&clock_widget,&my_clock.minute, minute_widget_element_config);
SecondWidgetElement clock_second_widget_element = SecondWidgetElement(&clock_widget,&my_clock.second, second_widget_element_config);



// ####################
int main()
{
    stdio_init_all();

#ifdef SHOW_CONSOLE_WIDGET
    xTaskCreate(clock_controller_console_widget_task, "manager_widget_task", 256, NULL, 13, &my_focus_manager_console_widget.task_handle);
    xTaskCreate(main_clock_console_widget_task, "main_clock_widget_task", 256, NULL, 12, &my_main_clock_console_widget.task_handle);

#endif
#ifdef SHOW_MONITORING_WIDGET
    xTaskCreate(I2C_display_gate_keeper_task, "I2C_gate_keeper_task", 256, NULL, 5, NULL);
    xTaskCreate(controller_monitoring_widget_task, "ctrl_monit", 256, NULL, 10, &controller_monitoring_widget.task_handle);
    xTaskCreate(clock_monitoring_widget_task, "clk_monit", 256, NULL, 10, &clock_monitoring_widget.task_handle);
#endif

    xTaskCreate(central_switch_process_irq_event_task, "central_switch_process_irq_event_task", 256, NULL, 25, NULL);
    xTaskCreate(encoder_process_irq_event_task, "encoder_process_irq_event_task", 256, NULL, 25, NULL);

    xTaskCreate(one_second_timer_task, "one_second_timer_task", 256, NULL, 20, NULL);
    xTaskCreate(my_clock_main_task, "clock_task", 256, &p1, 21, NULL);
    xTaskCreate(my_clock_controlled_hour_task, "hour_task", 256, NULL, 22, NULL);
    xTaskCreate(my_clock_controlled_minute_task, "minute_task", 256, NULL, 22, NULL);
    xTaskCreate(my_clock_controlled_second_task, "second_task", 256, NULL, 22, NULL);
    xTaskCreate(my_clock_controller_task, "clock_controller_task", 256, NULL, 8, &my_clock_controller.task_handle);

    xTaskCreate(blinker_task, "blinker", 256, &p0, 25, NULL);

    xTaskCreate(SPI_display_gate_keeper_task, "SPI_gate_keeper_task", 256, &p3, 5, NULL);

    xTaskCreate(SPI_hour_text_widget_task, "SPI_hour", 256, NULL, 25, &hour_text_widget.task_handle);
    xTaskCreate(SPI_minute_text_widget_task, "SPI_minute", 256, NULL, 25, &minute_text_widget.task_handle);
    xTaskCreate(SPI_second_text_widget_task, "SPI_second", 256, NULL, 25, &second_text_widget.task_handle);

    xTaskCreate(clock_widget_task, "clock_widget_task", 256, &p4, 24, &clock_widget.task_handle);
    xTaskCreate(SPI_clock_hour_widget_element_task, "SPI_hour_widget_element", 256, &p7, 20, &clock_hour_widget_element.task_handle);
    xTaskCreate(SPI_clock_minute_widget_element_task, "SPI_minute_widget_element", 256, &p6, 20, &clock_minute_widget_element.task_handle);
    xTaskCreate(SPI_clock_second_widget_element_task, "SP_second_widget_element", 256, &p5, 20, &clock_second_widget_element.task_handle);

    xTaskCreate(idle_task, "idle_task", 256, NULL, 0, NULL);
    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
