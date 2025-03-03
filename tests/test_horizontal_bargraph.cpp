/**
 * @file test_horizontal_bargraph.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ssd1306.h"
#include "probe.h"
#include "widget_bargraph.h"

// #define PRINT_DEBUG

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

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

void simulate_values(ModelBargraph *model)
{
    for (int i = 0; i < model->number_of_bar; i++)
    {
        model->values[i] += i + 1;
        if ((model->values[i] >= model->max_value) or (model->values[i] <= model->min_value))
            model->values[i] = model->min_value;
#ifdef PRINT_DEBUG
        printf("[%d]= %d, ", i, model->values[i]);
#endif
    }
#ifdef PRINT_DEBUG
    printf("\n");
#endif
    // model->process_control_event(); // pas besoin UI
}

struct_ConfigGraphicFramebuffer horizontal_bargraph_cfg = {
    .frame_width = 56,
    .frame_height = 56,
    .fg_color = PixelColor::WHITE,
    .bg_color = PixelColor::BLACK};

HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
SSD1306 display = SSD1306(&master, cfg_ssd1306);
ModelBargraph my_model = ModelBargraph(7, 0, 100);
WidgetHorizontalBargraph my_widget = WidgetHorizontalBargraph(&my_model,
                                                              &display,
                                                              horizontal_bargraph_cfg,
                                                              20, 0,
                                                              true);

int main()
{
#ifdef PRINT_DEBUG
    stdio_init_all();
#endif
    display.clear_device_screen_buffer();

    while (true)
    {
        simulate_values(&my_model); // TODO voir pour essayer des configs differentes avec range -10 +10 et simulate value en mm tps
        my_widget.draw(); // suffisant car pas besoin UI
        sleep_ms(100);
    }

    return 0;
}
