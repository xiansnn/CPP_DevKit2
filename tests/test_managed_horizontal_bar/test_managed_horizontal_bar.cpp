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

#include "device_SSD1306/ssd1306.h"
#include "device_KY040/ky040.h"
#include "utilities_debug_probe/probe.h"

#include "t_managed_horizontal_bar_models.cpp"
#include "t_managed_horizontal_bar_widgets.cpp"
#include "t_managed_horizontal_bar_manager.cpp"

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define CENTRAL_SWITCH_TIME_OUT_us 3000000

Probe pr_D4 = Probe(4);

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

struct_ConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_us = CENTRAL_SWITCH_TIME_OUT_us,
    .active_lo = true};

struct_ConfigSwitchButton cfg_encoder_clk{
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

MyControlledHorizontalBarModel my_horizontal_bar_model_1 = MyControlledHorizontalBarModel("HBar1", 0, 10, true, 1);
MyControlledHorizontalBarModel my_horizontal_bar_model_2 = MyControlledHorizontalBarModel("HBar2", -10, 10, false, 1);
MyControlledHorizontalBarModel my_horizontal_bar_model_3 = MyControlledHorizontalBarModel("HBar3", -20, 3, false, 1);

HW_I2C_Master master = HW_I2C_Master(cfg_i2c);

SSD1306 display = SSD1306(&master, cfg_ssd1306);

struct_ConfigGraphicWidget h_bar1_with_focus_cfg{
    .pixel_frame_width = 100,
    .pixel_frame_height = 8,
    .fg_color = PixelColor::WHITE,
    .bg_color = PixelColor::BLACK,
    .widget_anchor_x = 10,
    .widget_anchor_y = 8,
    .widget_with_border = true};
struct_ConfigGraphicWidget h_bar2_with_focus_cfg{
    .pixel_frame_width = 100,
    .pixel_frame_height = 8,
    .fg_color = PixelColor::WHITE,
    .bg_color = PixelColor::BLACK,
    .widget_anchor_x = 10,
    .widget_anchor_y = 24,
    .widget_with_border = true};
struct_ConfigGraphicWidget h_bar3_with_focus_cfg{
    .pixel_frame_width = 100,
    .pixel_frame_height = 8,
    .fg_color = PixelColor::WHITE,
    .bg_color = PixelColor::BLACK,
    .widget_anchor_x = 10,
    .widget_anchor_y = 40,
    .widget_with_border = true};

My_Horizontal_Bar_Widget_With_Focus horizontal_bar_1 = My_Horizontal_Bar_Widget_With_Focus(&my_horizontal_bar_model_1,
                                                                                           &display,
                                                                                           10, 0,
                                                                                           h_bar1_with_focus_cfg);
My_Horizontal_Bar_Widget_With_Focus horizontal_bar_2 = My_Horizontal_Bar_Widget_With_Focus(&my_horizontal_bar_model_2,
                                                                                           &display,
                                                                                           +10, -10,
                                                                                           h_bar2_with_focus_cfg);
My_Horizontal_Bar_Widget_With_Focus horizontal_bar_3 = My_Horizontal_Bar_Widget_With_Focus(&my_horizontal_bar_model_3,
                                                                                           &display,
                                                                                           3, -20,
                                                                                           h_bar3_with_focus_cfg);

int main()
{
    stdio_init_all();

    ky040.update_UI_control_event_processor(manager_process_control_event);
    ky040.update_current_controlled_object(&my_horizontal_bar_model_1);

    manager.add_managed_model(&my_horizontal_bar_model_1);
    manager.add_managed_model(&my_horizontal_bar_model_2);
    manager.add_managed_model(&my_horizontal_bar_model_3);

    display.clear_device_screen_buffer();

    while (true)
    {
        manager.process_control_event(ky040.process_central_switch_event());

        pr_D4.hi();
        horizontal_bar_1.draw();
        pr_D4.lo();

        pr_D4.hi();
        horizontal_bar_2.draw();
        pr_D4.lo();

        pr_D4.hi();
        horizontal_bar_3.draw();
        pr_D4.lo();

        /// - sleep for 20ms
        sleep_ms(20);
    }

    return 0;
}
