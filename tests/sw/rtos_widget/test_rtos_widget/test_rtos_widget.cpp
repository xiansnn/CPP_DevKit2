/**
 * @file test_rtos_extended_text_and_graph_widgets.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-12-27
 *
 * @copyright Copyright (c) 2025
 *
 */

 //TODO add blinking features
 //TODO make conditional compilation for focus indicator widget


#include "sw/ui_core/rtos_ui_core.h"
#include "sw/widget/rtos_widget.h"
#include "t_rtos_widget_text_and_graph_widgets.h"
#include "t_rtos_widget_main_model_classes.h"
#include "t_rtos_widget_config.h"
#include "t_rtos_widget_encoder_controller.h"
#include "t_rtos_widget_main_model_tasks.h"
#include "t_rtos_widget_ST7735_display_setup.h"
#include "t_rtos_widget_SSD1306_display_setup.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
// Probe p2 = Probe(2);
// Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

//  SSD1306 setup
rtos_HW_I2C_Master i2c_master = rtos_HW_I2C_Master(cfg_i2c);
rtos_SSD1306 left_display = rtos_SSD1306(&i2c_master, cfg_left_screen);
rtos_SSD1306 right_display = rtos_SSD1306(&i2c_master, cfg_right_screen);
rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper = rtos_GraphicDisplayGateKeeper();

//  ST7735 setup
rtos_HW_SPI_Master spi_master = rtos_HW_SPI_Master(cfg_spi,
                                                   DMA_IRQ_0, end_of_TX_DMA_xfer_handler);
rtos_ST7735 color_display = rtos_ST7735(&spi_master, cfg_st7735);
rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper = rtos_GraphicDisplayGateKeeper();

//   widgets

my_text_widget ST7735_values_widget = my_text_widget(&color_display, ST7735_values_config,
                                                     ST7735_TEXT_CANVAS_FORMAT, nullptr);
my_visu_widget ST7735_graph_widget = my_visu_widget(&color_display, ST7735_graph_config,
                                                    ST7735_GRAPHICS_CANVAS_FORMAT, nullptr);
my_text_widget SSD1306_values_widget = my_text_widget(&left_display, SSD1306_values_config,
                                                      SSD1306_CANVAS_FORMAT, nullptr);
my_visu_widget SSD1306_graph_widget = my_visu_widget(&right_display, SSD1306_graph_config,
                                                     SSD1306_CANVAS_FORMAT, nullptr);

//  main model and tasks
my_model my_rtos_model = my_model();

my_PositionController position_controller = my_PositionController(true);
my_position_controller_widget SPI_focus_indicator_widget = my_position_controller_widget(&color_display, focus_indicator_config, ST7735_TEXT_CANVAS_FORMAT, &position_controller);

//  KY040 encoder controller setup
rtos_SwitchButton central_switch = rtos_SwitchButton(GPIO_CENTRAL_SWITCH,
                                                     &ky040_encoder_irq_call_back, position_controller.control_event_input_queue,
                                                     cfg_central_switch);
rtos_RotaryEncoder encoder = rtos_RotaryEncoder(GPIO_ENCODER_CLK, GPIO_ENCODER_DT,
                                                &ky040_encoder_irq_call_back, position_controller.control_event_input_queue,
                                                cfg_encoder_clk);

//  ------------------------------- main() -------------------------------------------------------------------
int main()
{
    stdio_init_all();

    xTaskCreate(central_switch_process_irq_event_task, "central_switch_process_irq_event_task", 256, NULL, 25, NULL);
    xTaskCreate(encoder_process_irq_event_task, "encoder_process_irq_event_task", 256, NULL, 25, NULL);

    xTaskCreate(angle_evolution_task, "periodic_task", 256, &p1, 20, NULL);
    xTaskCreate(my_model_task, "model_task", 256, &p1, 20, NULL); // 4us pour SPI_graph_widget_task, 12us SPI_values_widget_task, I2C_right_graph_widget_task, 16us pour I2C_left_values_widget_task

    xTaskCreate(position_controller_task, "position_controller_task", 256, &p5, 8, &position_controller.task_handle);
    xTaskCreate(controlled_position_task, "H_task", 256, &my_rtos_model.x_pos, 8, &my_rtos_model.x_pos.task_handle);
    xTaskCreate(controlled_position_task, "V_task", 256, &my_rtos_model.y_pos, 8, &my_rtos_model.y_pos.task_handle);
    xTaskCreate(controlled_position_task, "angle_task", 256, &my_rtos_model.angle, 8, &my_rtos_model.angle.task_handle);
    
    xTaskCreate(SPI_graph_widget_task, "graph_widget_task", 256, &p4, 13, &ST7735_graph_widget.task_handle);               // durée: 8.23ms + 14ms xfer SPI
    xTaskCreate(SPI_values_widget_task, "values_widget_task", 256, &p4, 12, &ST7735_values_widget.task_handle);            // durée 5,6 ms + 3,8ms xfer SPI (depends on font size)
    xTaskCreate(SPI_focus_widget_task, "focus_widget_task", 256, &p4, 12, &SPI_focus_indicator_widget.task_handle);        // durée 5,6 ms + 3,8ms xfer SPI (depends on font size)
    xTaskCreate(I2C_right_graph_widget_task, "right_graph_widget_task", 256, &p4, 11, &SSD1306_graph_widget.task_handle);  // 368us + 22.2ms xfer I2C
    xTaskCreate(I2C_left_values_widget_task, "left_values_widget_task", 256, &p4, 10, &SSD1306_values_widget.task_handle); // 2.69ms + 6.5ms xfer I2C (depends on font size)

    xTaskCreate(SPI_display_gate_keeper_task, "SPI_gate_keeper_task", 256, &p6, 5, NULL);
    xTaskCreate(I2C_display_gate_keeper_task, "I2C_gate_keeper_task", 256, &p7, 5, NULL);

    xTaskCreate(idle_task, "idle_task", 256, &p0, 0, NULL);
    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
