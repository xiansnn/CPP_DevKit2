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

#include "device/SSD1306/ssd1306.h"
#include "sw/widget_bargraph/widget_bargraph.h"
#include "utilities/probe/probe.h"

// #define PRINT_DEBUG

#define CANVAS_FORMAT CanvasFormat::MONO_VLSB

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
    std::vector<int> current_values = model->get_values();
    for (int i = 0; i < model->number_of_bar; i++)
    {
        current_values[i] += i + 1;
        if ((current_values[i] >= model->max_value) or (current_values[i] <= model->min_value))
            current_values[i] = model->min_value;
#ifdef PRINT_DEBUG
        printf("[%d]= %d, ", i, current_values[i]);
#endif
    }
#ifdef PRINT_DEBUG
    printf("\n");
#endif
    model->update_values(current_values);
}

struct_ConfigGraphicWidget horizontal_bargraph_cfg = {
    .pixel_frame_width = 56,
    .pixel_frame_height = 56,
    .fg_color = ColorIndex::WHITE,
    .bg_color = ColorIndex::BLACK,
    .widget_anchor_x = 20,
    .widget_anchor_y = 0,
    .widget_with_border = true};

int main()
{
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);
    ModelBargraph my_model = ModelBargraph(7, 0, 100);
    WidgetHorizontalBargraph my_widget = WidgetHorizontalBargraph(&my_model,
                                                                  &display,
                                                                  horizontal_bargraph_cfg, 
                                                                  CANVAS_FORMAT);

#ifdef PRINT_DEBUG
    stdio_init_all();
#endif
    display.clear_device_screen_buffer();

    while (true)
    {
        simulate_values(&my_model);
        my_model.draw_refresh_all_attached_widgets();
        sleep_ms(100);
    }

    return 0;
}
