#include "t_rtos_blinking_led_config.h"
#include "t_rtos_blinking_led_display_setup.h"
#include "t_rtos_blinking_led_encoder_controller.h"
#include "t_rtos_blinking_led_main_classes.h"
#include "t_rtos_blinking_led_main_tasks.h"
#include "t_rtos_blinking_led_monitoring_widgets.h"

// #define SHOW_DUMMY_WIDGET

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

// TODO classes LED ;  blinker; model attaché à LED; widget attaché à focus manager

// ##### main classes #####
myClockController my_clock_controller = myClockController(true);
myMainClock my_clock = myMainClock();

// ##### ST7735 setup #####
rtos_HW_SPI_Master spi_master = rtos_HW_SPI_Master(cfg_spi,
                                                   DMA_IRQ_0, end_of_TX_DMA_xfer_handler);
rtos_ST7735 color_display = rtos_ST7735(&spi_master, cfg_st7735);
rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper = rtos_GraphicDisplayGateKeeper();

// ##### SSD1306 setup #####
rtos_HW_I2C_Master i2c_master = rtos_HW_I2C_Master(cfg_i2c);
rtos_SSD1306 left_display = rtos_SSD1306(&i2c_master, cfg_left_screen);
rtos_SSD1306 right_display = rtos_SSD1306(&i2c_master, cfg_right_screen);
rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper = rtos_GraphicDisplayGateKeeper();

// ##### KY040 encoder controller setup #####
rtos_SwitchButton central_switch = rtos_SwitchButton(GPIO_CENTRAL_SWITCH,
                                                     &ky040_encoder_irq_call_back, my_clock_controller.control_event_input_queue,
                                                     cfg_central_switch);
rtos_RotaryEncoder encoder = rtos_RotaryEncoder(GPIO_ENCODER_CLK, GPIO_ENCODER_DT,
                                                &ky040_encoder_irq_call_back, my_clock_controller.control_event_input_queue,
                                                cfg_encoder_clk);

// ##### Widgets #####
#ifdef SHOW_DUMMY_WIDGET
focus_dummy_widget my_focus_manager_dummy_widget = focus_dummy_widget(&my_clock_controller, nullptr);
clock_dummy_widget my_main_clock_dummy_widget = clock_dummy_widget(&my_clock, nullptr);
clock_hand_dummy_widget hour_hand_dummy_widget = clock_hand_dummy_widget(&my_clock.hour, nullptr);
clock_hand_dummy_widget minute_hand_dummy_widget = clock_hand_dummy_widget(&my_clock.minute, nullptr);
clock_hand_dummy_widget second_hand_dummy_widget = clock_hand_dummy_widget(&my_clock.second, nullptr);
#endif
my_controller_monitoring_widget controller_monitoring_widget = my_controller_monitoring_widget(&right_display, monitoring_text_cfg, CanvasFormat::MONO_VLSB, &my_clock_controller);
my_clock_monitoring_widget clock_monitoring_widget = my_clock_monitoring_widget(&left_display, monitoring_text_cfg, CanvasFormat::MONO_VLSB, &my_clock);

// ####################
int main()
{
#ifdef SHOW_DUMMY_WIDGET
    stdio_init_all();
    xTaskCreate(clock_controller_dummy_widget_task, "manager_widget_task", 256, &p4, 13, &my_focus_manager_dummy_widget.task_handle);
    xTaskCreate(main_clock_dummy_widget_task, "main_clock_widget_task", 256, &p5, 12, &my_main_clock_dummy_widget.task_handle);
    xTaskCreate(main_clock_hand_widget_task, "hour_widget_task", 256, &hour_hand_dummy_widget, 11, &hour_hand_dummy_widget.task_handle);
    xTaskCreate(main_clock_hand_widget_task, "minute_widget_task", 256, &minute_hand_dummy_widget, 11, &minute_hand_dummy_widget.task_handle);
    xTaskCreate(main_clock_hand_widget_task, "hour_widget_task", 256, &second_hand_dummy_widget, 11, &second_hand_dummy_widget.task_handle);

    xTaskCreate(clock_controller_dummy_widget_task, "manager_widget_task", 256, &p4, 13, &my_focus_manager_dummy_widget.task_handle);
    xTaskCreate(main_clock_dummy_widget_task, "main_clock_widget_task", 256, &p5, 12, &my_main_clock_dummy_widget.task_handle);
    xTaskCreate(main_clock_hand_widget_task, "hour_widget_task", 256, &hour_hand_dummy_widget, 11, &hour_hand_dummy_widget.task_handle);
    xTaskCreate(main_clock_hand_widget_task, "minute_widget_task", 256, &minute_hand_dummy_widget, 11, &minute_hand_dummy_widget.task_handle);
    xTaskCreate(main_clock_hand_widget_task, "hour_widget_task", 256, &second_hand_dummy_widget, 11, &second_hand_dummy_widget.task_handle);
#endif

    xTaskCreate(central_switch_process_irq_event_task, "central_switch_process_irq_event_task", 256, NULL, 25, NULL);
    xTaskCreate(encoder_process_irq_event_task, "encoder_process_irq_event_task", 256, NULL, 25, NULL);

    xTaskCreate(one_second_timer_task, "one_second_timer_task", 256, &p1, 20, NULL);
    xTaskCreate(my_main_clock_task, "clock_task", 256, &p1, 20, NULL);
    xTaskCreate(my_main_clock_hour_task, "hour_task", 256, &p1, 20, NULL);
    xTaskCreate(my_main_clock_minute_task, "minute_task", 256, &p1, 20, NULL);
    xTaskCreate(my_main_clock_second_task, "second_task", 256, &p1, 20, NULL);

    xTaskCreate(my_clock_controller_task, "focus_led_manager_task", 256, &p6, 8, &my_clock_controller.task_handle);

    xTaskCreate(controller_monitoring_widget_task, "ctrl_monit", 256, &controller_monitoring_widget, 10, &controller_monitoring_widget.task_handle);
    xTaskCreate(clock_monitoring_widget_task, "clk_monit", 256, &clock_monitoring_widget, 10, &clock_monitoring_widget.task_handle);

    // xTaskCreate(SPI_display_gate_keeper_task, "SPI_gate_keeper_task", 256, &p6, 5, NULL);
    xTaskCreate(I2C_display_gate_keeper_task, "I2C_gate_keeper_task", 256, &p7, 5, NULL);

    xTaskCreate(idle_task, "idle_task", 256, &p0, 0, NULL);
    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
