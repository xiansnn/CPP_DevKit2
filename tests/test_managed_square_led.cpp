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
#include "widget_square_led.h"
#include "ssd1306.h"
#include "probe.h"
#include "ky040.h"

#include "test_managed_square_led/t_managed_square_led_manager.cpp"
#include "test_managed_square_led/t_managed_square_led_models.cpp"
#include "test_managed_square_led/t_managed_square_led_widgets.cpp"

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define CENTRAL_SWITCH_TIME_OUT_us 2000000

/// @brief ########## Debug/Observer Probe for logic analyser section ##########
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D1 = Probe(1);

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
struct_SwitchButtonConfig cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_us = CENTRAL_SWITCH_TIME_OUT_us,
    .active_lo = true};

/// @brief config of the rotary encoder clock signal
struct_SwitchButtonConfig cfg_encoder_clk{
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
    pr_D1.pulse_us(10);
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
void manager_event_processor(UIControlEvent event)
{
    manager.process_control_event(event);
};


/**
 * @brief Example of main program of a managed SquareLED widget.
 */
int main()
{
    ky040.update_UI_control_event_processor(manager_event_processor);
    pr_D4.hi(); // start logic probe 4
    /// main steps:
    stdio_init_all();
    /// 1- create I2C bus hw peripheral
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    /// 2- create the OLED display, clean up the screen
    SSD1306 display = SSD1306(&master, cfg_ssd1306);
    pr_D5.hi();
    display.clear_pixel_buffer();
    // display.rect(0, 0, 128, 64); //uncomment if a border is needed
    display.show();
    pr_D5.lo();
    /// 3- create 3 MySquareLedModel  as displayed object for MySquareLedWidget
    MySquareLedModel test_model_1 = MySquareLedModel("TM1");
    MySquareLedModel test_model_2 = MySquareLedModel("TM2");
    MySquareLedModel test_model_3 = MySquareLedModel("TM3");
    /// 4- create 3 MySquareLedWidget and the MyFocusLedWidget
    MySquareLedWidget square_led_1 = MySquareLedWidget(&test_model_1, &display, 20, 8, 6, 0);
    MySquareLedWidget square_led_2 = MySquareLedWidget(&test_model_2, &display, 20, 8, 6, 8);
    MySquareLedWidget square_led_3 = MySquareLedWidget(&test_model_3, &display, 20, 8, 6, 16);
    MyFocusLedWidget focus_led_1 = MyFocusLedWidget(&test_model_1, &display, 3, 8, 0, 0);
    MyFocusLedWidget focus_led_2 = MyFocusLedWidget(&test_model_2, &display, 3, 8, 0, 8);
    MyFocusLedWidget focus_led_3 = MyFocusLedWidget(&test_model_3, &display, 3, 8, 0, 16);

    /// 5- set led_is_blinking period of the MyFocusLedWidget
    focus_led_1.set_blink_us(200000);
    focus_led_2.set_blink_us(200000);
    focus_led_3.set_blink_us(200000);
    /// 6- create a test_switch_button
    ky040.update_current_controlled_object(&test_model_1);

    /// 8- populate manager with managed objects
    manager.add_managed_model(&test_model_1);
    manager.add_managed_model(&test_model_2);
    manager.add_managed_model(&test_model_3);
    pr_D4.lo(); // end logic probe 4

    while (true)
    /// 9- start infinite loop
    {
        pr_D5.hi(); // start logic probe 5
        /// - get central_switch event and give it to the manager .
        manager.process_control_event(ky040.process_central_switch_event());

        /// - refresh the widgets
        square_led_1.draw_refresh();
        square_led_2.draw_refresh();
        square_led_3.draw_refresh();
        focus_led_1.draw_refresh();
        focus_led_2.draw_refresh();
        focus_led_3.draw_refresh();
        pr_D5.lo(); // end logic probe 5
        /// - sleep for 20ms
        sleep_ms(20);
    }

    return 0;
}
