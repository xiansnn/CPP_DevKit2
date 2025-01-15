/**
 * @file test_managed_horizontal_bar.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ssd1306.h"
#include "probe.h"
#include "ky040.h"

#include "test_managed_horizontal_bar/t_managed_horizontal_bar_models.cpp"
#include "test_managed_horizontal_bar/t_managed_horizontal_bar_widgets.cpp"
#include "test_managed_horizontal_bar/t_managed_horizontal_bar_manager.cpp"

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define CENTRAL_SWITCH_TIME_OUT_us 3000000

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D1 = Probe(1);

struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};

struct_ConfigSSD1306 cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

struct_SwitchButtonConfig cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_us = CENTRAL_SWITCH_TIME_OUT_us,
    .active_lo = true};

struct_SwitchButtonConfig cfg_encoder_clk{
    .debounce_delay_us = 5000};

void shared_irq_call_back(uint gpio, uint32_t event_mask);

KY040 ky040 = KY040(CENTRAL_SWITCH_GPIO,
                    ENCODER_CLK_GPIO,
                    ENCODER_DT_GPIO,
                    &shared_irq_call_back,
                    cfg_central_switch,
                    cfg_encoder_clk);

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

MyManager manager = MyManager(&ky040, false);

void manager_process_control_event(UIControlEvent event)
{
    manager.process_control_event(event);
};

int main()
{
    ky040.update_UI_control_event_processor(manager_process_control_event);

    stdio_init_all();

    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);

    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    display.clear_pixel_buffer();
    display.show();

    MyHorizontalBarModel my_horizontal_bar_model_1 = MyHorizontalBarModel("HBar1", 0, 10, true, 1);
    MyHorizontalBarModel my_horizontal_bar_model_2 = MyHorizontalBarModel("HBar2", -10, 10, false, 1);
    MyHorizontalBarModel my_horizontal_bar_model_3 = MyHorizontalBarModel("HBar3", -20, 3, false, 1);

    ky040.update_current_controlled_object(&my_horizontal_bar_model_1);
    manager.add_managed_model(&my_horizontal_bar_model_1);
    manager.add_managed_model(&my_horizontal_bar_model_2);
    manager.add_managed_model(&my_horizontal_bar_model_3);

    MyHorizontalBarWidgetWithFocus horizontal_bar_1 = MyHorizontalBarWidgetWithFocus(&my_horizontal_bar_model_1, &display, 10, 0, 100, 8, 10, 8);
    MyHorizontalBarWidgetWithFocus horizontal_bar_2 = MyHorizontalBarWidgetWithFocus(&my_horizontal_bar_model_2, &display, +10, -10, 100, 8, 10, 24);
    MyHorizontalBarWidgetWithFocus horizontal_bar_3 = MyHorizontalBarWidgetWithFocus(&my_horizontal_bar_model_3, &display, 3, -20, 100, 8, 10, 40);

    while (true)
    /// 9- start infinite loop
    {
        pr_D5.hi(); // start logic probe 5
        /// - get central_switch event and give it to the manager .
        manager.process_control_event(ky040.process_central_switch_event());

        pr_D4.hi();
        horizontal_bar_1.draw_refresh();
        pr_D4.lo();
        pr_D4.hi();
        horizontal_bar_2.draw_refresh();
        pr_D4.lo();
        pr_D4.hi();
        horizontal_bar_3.draw_refresh();
        pr_D4.lo();

        pr_D5.lo(); // end logic probe 5
        /// - sleep for 20ms
        sleep_ms(20);
    }

    return 0;
}
