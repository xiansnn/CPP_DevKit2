/**
 * @file test_rtos_ssd1306_text_and_graph_widgets.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-12-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "sw/ui_core/rtos_ui_core.h"
#include "sw/widget/rtos_widget.h"
#include "device/SSD1306/ssd1306.h"
#include "device/ST7735/st7735.h"
#include "device/rotary_encoder/rtos_rotary_encoder.h"
#include "t_rtos_common_text_and_graph_widgets.h"
#include "t_rtos_all_device_roll_control.h"
#include "t_rtos_all_device_defines.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
// Probe p2 = Probe(2);
// Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

//==========================display gatekeeper===============
rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper = rtos_GraphicDisplayGateKeeper();
rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper = rtos_GraphicDisplayGateKeeper();

//==========================Master I2C========================
void i2c_irq_handler();
struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE,
    .i2c_tx_master_handler = i2c_irq_handler};
rtos_HW_I2C_Master master = rtos_HW_I2C_Master(cfg_i2c);
void i2c_irq_handler()
{
    master.i2c_dma_isr();
};

//==================SSD1306 left screen===================
struct_ConfigSSD1306 cfg_left_screen{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};
rtos_SSD1306 left_display = rtos_SSD1306(&master, cfg_left_screen);

//==================SSD1306 right screen===================
struct_ConfigSSD1306 cfg_right_screen{
    .i2c_address = 0x3D,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};
rtos_SSD1306 right_display = rtos_SSD1306(&master, cfg_right_screen);

//==========================Master SPI========================
struct_ConfigMasterSPI cfg_spi = {
    .spi = spi1,
    .sck_pin = 10,
    .tx_pin = 11,
    .rx_pin = 12,
    .cs_pin = 13,
    .baud_rate_Hz = 10 * 1000 * 1000};
void end_of_TX_DMA_xfer_handler();
rtos_HW_SPI_Master spi_master = rtos_HW_SPI_Master(cfg_spi,
                                                   DMA_IRQ_0, end_of_TX_DMA_xfer_handler);
void end_of_TX_DMA_xfer_handler()
{
    spi_master.spi_tx_dma_isr();
}

//================== ST7735===================
struct_ConfigST7735 cfg_st7735{
    .display_type = DEVICE_DISPLAY_TYPE,
    .backlight_pin = 5,
    .hw_reset_pin = 15,
    .dc_pin = 14,
    .rotation = DEVICE_DISPLAY_ROTATION};
rtos_ST7735 color_display = rtos_ST7735(&spi_master, cfg_st7735);

//-------------Model-----
my_model my_rtos_model = my_model();

//-------------ST7735-widgets-----
struct_ConfigTextWidget ST7735_title_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 0,
    .widget_anchor_y = 64,
    .font = font_12x16};
my_text_widget title = my_text_widget(&color_display, ST7735_title_config, ST7735_TEXT_CANVAS_FORMAT);

struct_ConfigTextWidget ST7735_values_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 68,
    .widget_anchor_y = 64,
    .font = font_12x16};
my_text_widget my_rtos_values_widget = my_text_widget(&color_display, ST7735_values_config, ST7735_TEXT_CANVAS_FORMAT, nullptr);

struct_ConfigGraphicWidget st7735_graph_config{
    .canvas_width_pixel = 128,
    .canvas_height_pixel = 56,
    .canvas_foreground_color = ColorIndex::YELLOW,
    .canvas_background_color = ColorIndex::RED,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};
my_visu_widget my_rtos_graph_widget = my_visu_widget(&color_display, st7735_graph_config, ST7735_GRAPHICS_CANVAS_FORMAT, nullptr);

//-------------SSD1306-widgets-----
struct_ConfigTextWidget SSD1306_title_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .font = font_12x16};
my_text_widget left_title = my_text_widget(&left_display, SSD1306_title_config, SSD1306_CANVAS_FORMAT);

struct_ConfigTextWidget SSD1306_values_config = {
    .number_of_column = 5,
    .number_of_line = 3,
    .widget_anchor_x = 68,
    .widget_anchor_y = 0,
    .font = font_12x16};
my_text_widget my_rtos_left_values_widget = my_text_widget(&left_display, SSD1306_values_config, SSD1306_CANVAS_FORMAT, nullptr);

struct_ConfigGraphicWidget ssd1306_graph_config{
    .canvas_width_pixel = 128,
    .canvas_height_pixel = 56,
    .canvas_foreground_color = ColorIndex::WHITE,
    .canvas_background_color = ColorIndex::BLACK,
    .widget_anchor_x = 0,
    .widget_anchor_y = 0,
    .widget_with_border = true};
my_visu_widget my_rtos_right_graph_widget = my_visu_widget(&right_display, ssd1306_graph_config, SSD1306_CANVAS_FORMAT, nullptr);

//---------------H V and angle Position Controller------------------------------
my_PositionController position_controller = my_PositionController(true);
struct_ConfigTextWidget position_controller_focus_config = {
    .number_of_column = 10,
    .number_of_line = 1,
    .widget_anchor_x = 4,
    .widget_anchor_y = 140,
    .font = font_12x16,
    .widget_with_border = true};
my_position_controller_widget focus_indicator = my_position_controller_widget(&color_display,position_controller_focus_config,ST7735_TEXT_CANVAS_FORMAT,&position_controller);
//-----KY040---------
//                   rtos_SwitchButton central_switch---------------------
struct_rtosConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1500,
    .time_out_delay_ms = SW_TIMEOUT_DELAY_ms};
void ky040_encoder_irq_call_back(uint gpio, uint32_t event_mask);
rtos_SwitchButton central_switch = rtos_SwitchButton(CENTRAL_SWITCH_GPIO,
                                                     &ky040_encoder_irq_call_back, position_controller.control_event_input_queue,
                                                     cfg_central_switch);
void central_switch_process_irq_event_task(void *)
{
    central_switch.rtos_process_IRQ_event();
}
//                   rtos_RotaryEncoder encoder-------------------------------
struct_rtosConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1000,
    .time_out_delay_ms = SW_TIMEOUT_DELAY_ms};
rtos_RotaryEncoder encoder = rtos_RotaryEncoder(ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                                &ky040_encoder_irq_call_back, position_controller.control_event_input_queue,
                                                cfg_encoder_clk);
void encoder_process_irq_event_task(void *)
{
    encoder.rtos_process_IRQ_event();
}
//                 ky040_encoder_irq_call_back--------------------------------
void ky040_encoder_irq_call_back(uint gpio, uint32_t event_mask)
{
    struct_SwitchButtonIRQData data;
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    data.current_time_us = time_us_32();
    data.event_mask = event_mask;
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    switch (gpio)
    {
    case CENTRAL_SWITCH_GPIO:
        xQueueSendFromISR(central_switch.IRQdata_input_queue, &data, &pxHigherPriorityTaskWoken);
        break;
    case ENCODER_CLK_GPIO:
        xQueueSendFromISR(encoder.IRQdata_input_queue, &data, &pxHigherPriorityTaskWoken);
        break;
    default:
        break;
    }
    portYIELD_FROM_ISR(&pxHigherPriorityTaskWoken);
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
};

//---------------TASKS-----------------
void idle_task(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void position_controller_task(void *probe)
{
    position_controller.add_managed_rtos_model(&my_rtos_model.angle);
    position_controller.add_managed_rtos_model(&my_rtos_model.y_pos);
    position_controller.add_managed_rtos_model(&my_rtos_model.x_pos);
    position_controller.notify_all_linked_widget_task();

    struct_ControlEventData local_event_data;
    BaseType_t global_timeout_condtion;
    while (true)
    {
        if (probe != NULL)
            ((Probe *)probe)->pulse_us(10);
        position_controller.process_event_and_time_out_condition(&position_controller, UI_MANAGER_TIMEOUT_DELAY_ms);
    }
};

void SPI_display_gate_keeper_task(void *probe)
{
    struct_WidgetDataToGateKeeper received_data_to_show;

    while (true)
    {
        xQueueReceive(SPI_display_gate_keeper.graphic_widget_data, &received_data_to_show, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        SPI_display_gate_keeper.receive_widget_data(received_data_to_show);
        if (probe != NULL)
            ((Probe *)probe)->lo();
    }
}

void SPI_graph_widget_task(void *probe)
{
    SPI_display_gate_keeper.send_clear_device_command(&color_display);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_rtos_graph_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&my_rtos_graph_widget);
    }
}

void SPI_values_widget_task(void *probe)
{
    my_text_widget title = my_text_widget(&color_display, ST7735_title_config, ST7735_TEXT_CANVAS_FORMAT);
    title.writer->write("ANGLEH_POSV_POS");
    SPI_display_gate_keeper.send_widget_data(&title);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_rtos_values_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&my_rtos_values_widget);
    }
}
void SPI_focus_widget_task(void *probe)
{

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        focus_indicator.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        SPI_display_gate_keeper.send_widget_data(&focus_indicator);
    }
}

void I2C_display_gate_keeper_task(void *probe)
{
    struct_WidgetDataToGateKeeper received_data_to_show;

    while (true)
    {
        xQueueReceive(I2C_display_gate_keeper.graphic_widget_data, &received_data_to_show, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        I2C_display_gate_keeper.receive_widget_data(received_data_to_show);
        if (probe != NULL)
            ((Probe *)probe)->lo();
    }
}

void I2C_right_graph_widget_task(void *probe)
{
    I2C_display_gate_keeper.send_clear_device_command(&right_display);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_rtos_right_graph_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        I2C_display_gate_keeper.send_widget_data(&my_rtos_right_graph_widget);
    }
}
void I2C_left_values_widget_task(void *probe)
{
    I2C_display_gate_keeper.send_clear_device_command(&left_display);
    my_text_widget left_title = my_text_widget(&left_display, SSD1306_title_config, SSD1306_CANVAS_FORMAT);
    left_title.writer->write("ANGLEH_POSV_POS");
    I2C_display_gate_keeper.send_widget_data(&left_title);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (probe != NULL)
            ((Probe *)probe)->hi();
        my_rtos_left_values_widget.draw();
        if (probe != NULL)
            ((Probe *)probe)->lo();
        I2C_display_gate_keeper.send_widget_data(&my_rtos_left_values_widget);
    }
}
void angle_evolution_task(void *probe) // periodic task
{
    struct_ControlEventData data;
    data.gpio_number = DUMMY_GPIO_FOR_PERIODIC_EVOLUTION;
    data.event = UIControlEvent::INCREMENT;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true)
    {
        if (probe != NULL)
            ((Probe *)probe)->pulse_us();
        xQueueSend(my_rtos_model.angle.control_event_input_queue, &data, portMAX_DELAY);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(REFRESH_PERIOD_ms));
    }
}

void my_model_task(void *probe)
{
    my_rtos_model.update_attached_rtos_widget(&my_rtos_right_graph_widget);
    my_rtos_model.update_attached_rtos_widget(&my_rtos_left_values_widget);
    my_rtos_model.update_attached_rtos_widget(&my_rtos_graph_widget);
    my_rtos_model.update_attached_rtos_widget(&my_rtos_values_widget);
    my_rtos_model.notify_all_linked_widget_task();

    while (true)
    {
        struct_ControlEventData data;
        while (true)
        {
            xQueueReceive(my_rtos_model.control_event_input_queue, &data, portMAX_DELAY);
            if (probe != NULL)
                ((Probe *)probe)->hi();
            my_rtos_model.process_control_event(data);
            if (probe != NULL)
                ((Probe *)probe)->lo();
        }
    }
}

void controlled_position_task(void *position)
{
    struct_ControlEventData received_control_event;
    my_ControlledRollPosition *center_position = (my_ControlledRollPosition *)position;
    while (true)
    {
        xQueueReceive(center_position->control_event_input_queue, &received_control_event, portMAX_DELAY);
        center_position->process_control_event(received_control_event);
    }
}

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

    xTaskCreate(SPI_graph_widget_task, "graph_widget_task", 256, &p4, 13, &my_rtos_graph_widget.task_handle);                   // durée: 8.23ms + 14ms xfer SPI
    xTaskCreate(SPI_values_widget_task, "values_widget_task", 256, &p4, 12, &my_rtos_values_widget.task_handle);                // durée 5,6 ms + 3,8ms xfer SPI (depends on font size)
    xTaskCreate(SPI_focus_widget_task, "focus_widget_task", 256, &p4, 12, &focus_indicator.task_handle);                // durée 5,6 ms + 3,8ms xfer SPI (depends on font size)
    xTaskCreate(I2C_right_graph_widget_task, "right_graph_widget_task", 256, &p4, 11, &my_rtos_right_graph_widget.task_handle); // 368us + 22.2ms xfer I2C
    xTaskCreate(I2C_left_values_widget_task, "left_values_widget_task", 256, &p4, 10, &my_rtos_left_values_widget.task_handle); // 2.69ms + 6.5ms xfer I2C (depends on font size)

    xTaskCreate(SPI_display_gate_keeper_task, "SPI_gate_keeper_task", 256, &p6, 5, NULL);
    xTaskCreate(I2C_display_gate_keeper_task, "I2C_gate_keeper_task", 256, &p7, 5, NULL);

    xTaskCreate(idle_task, "idle_task", 256, &p0, 0, NULL);
    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
