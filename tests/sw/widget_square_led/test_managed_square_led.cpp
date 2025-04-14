/**
 * @file test_managed_square_led.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "device/SSD1306/ssd1306.h"
#include "device/KY040/ky040.h"
#include "sw/widget_square_led/widget_square_led.h"
#include "t_managed_square_led_models.cpp"
#include "t_managed_square_led_manager.cpp"
#include "t_managed_square_led_widgets.cpp"
#include "utilities/probe/probe.h"

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define CENTRAL_SWITCH_TIME_OUT_us 2000000

/// @brief ########## Debug/Observer Probe for logic analyser section ##########

// Probe pr_D1 = Probe(1);

/// @brief ########## configuration section ##########

/// @brief  define i2c config
struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};

/// @brief define SSD1306 display config
struct_ConfigSSD1306 cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

/// @brief define central switch config
struct_ConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_us = CENTRAL_SWITCH_TIME_OUT_us,
    .active_lo = true};

/// @brief config of the rotary encoder clock signal
struct_ConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000};

void shared_irq_call_back(uint gpio, uint32_t event_mask);

/// @brief create KY040 rotary encoder

KY040 ky040 = KY040(CENTRAL_SWITCH_GPIO,
                    ENCODER_CLK_GPIO,
                    ENCODER_DT_GPIO,
                    &shared_irq_call_back,
                    cfg_central_switch,
                    cfg_encoder_clk);

/// @brief define the ISR associated with the clock signal of the rotary encoder
/// @param gpio The gpio number connected to the clock signal
/// @param event_mask the IRQ mask, default to GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE
void shared_irq_call_back(uint gpio, uint32_t event_mask)
{
    // pr_D1.pulse_us(10);
    switch (gpio)
    {
    case ENCODER_CLK_GPIO:
        ky040.process_encoder_IRQ(event_mask);
        break;

    default:
        printf("unknown IRQ\n");
        break;
    };
}
/// 7- create a MyManager connected to the rotary encoder.
MyManager manager = MyManager(&ky040);

/// @brief global function necessary to convert bound fonction for control_event_processor_t
/// @param event
void manager_process_control_event(UIControlEvent event)
{
    manager.process_control_event(event);
};

/**
 * @brief Example of main program of a managed SquareLED widget.
 */
int main()
{
    ky040.update_UI_control_event_processor(manager_process_control_event);

    stdio_init_all();

    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);
    display.clear_device_screen_buffer();

    MyManagedSquareLedModel test_model_1 = MyManagedSquareLedModel("TM1");
    MyManagedSquareLedModel test_model_2 = MyManagedSquareLedModel("TM2");
    MyManagedSquareLedModel test_model_3 = MyManagedSquareLedModel("TM3");

    struct_ConfigGraphicWidget square_led_1_cfg{
        .pixel_frame_width = 50,
        .pixel_frame_height = 8,
        .fg_color = ColorIndex::WHITE,
        .bg_color = ColorIndex::BLACK,
        .widget_anchor_x = 6,
        .widget_anchor_y = 8,
        .widget_with_border = false};
    MySquareLEDWidgetWithFocus square_led_1 = MySquareLEDWidgetWithFocus(&test_model_1, &display, square_led_1_cfg,CANVAS_FORMAT);

    struct_ConfigGraphicWidget square_led_2_cfg{
        .pixel_frame_width = 50,
        .pixel_frame_height = 8,
        .fg_color = ColorIndex::WHITE,
        .bg_color = ColorIndex::BLACK,
        .widget_anchor_x = 6,
        .widget_anchor_y = 24,
        .widget_with_border = false};
    MySquareLEDWidgetWithFocus square_led_2 = MySquareLEDWidgetWithFocus(&test_model_2, &display, square_led_2_cfg,CANVAS_FORMAT);

    struct_ConfigGraphicWidget square_led_3_cfg{
        .pixel_frame_width = 50,
        .pixel_frame_height = 8,
        .fg_color = ColorIndex::WHITE,
        .bg_color = ColorIndex::BLACK,
        .widget_anchor_x = 6,
        .widget_anchor_y = 40,
        .widget_with_border = false};
    MySquareLEDWidgetWithFocus square_led_3 = MySquareLEDWidgetWithFocus(&test_model_3, &display, square_led_3_cfg,CANVAS_FORMAT);

    ky040.update_current_controlled_object(&test_model_1);

    manager.add_managed_model(&test_model_1);
    manager.add_managed_model(&test_model_2);
    manager.add_managed_model(&test_model_3);

    while (true)
    {
        // pr_D1.hi();
        /// - get central_switch event and give it to the manager .
        manager.process_control_event(ky040.process_central_switch_event());
        /// - refresh the widgets
        square_led_1.draw();
        square_led_2.draw();
        square_led_3.draw();

        // pr_D1.lo();
        /// - sleep for 20ms
        sleep_ms(20);
    }

    return 0;
}
