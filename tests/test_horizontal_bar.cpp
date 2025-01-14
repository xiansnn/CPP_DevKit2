/**
 * @file test_horizontal_bar.cpp
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

#include "widget_horizontal_bar.h"
#include "ui_core.h"


Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);




#define MAX_VALUE 10
#define MIN_VALUE -10


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

class MyHorizontalBarModel : public UIControlledIncrementalValue
{
private:
    /* data */
public:
    MyHorizontalBarModel(int _min_value, int _max_value, bool _is_wrappable, int _increment);
    ~MyHorizontalBarModel();
    void process_control_event(UIControlEvent _event);
};

MyHorizontalBarModel::MyHorizontalBarModel(int _min_value, int _max_value, bool _is_wrappable, int _increment)
    : UIControlledIncrementalValue(_min_value, _max_value, _is_wrappable, _increment)
{
}

MyHorizontalBarModel::~MyHorizontalBarModel()
{
}

void MyHorizontalBarModel::process_control_event(UIControlEvent _event)
{
    switch (_event)
    {
    case UIControlEvent::INCREMENT:
        this->increment_value();
        break;
    case UIControlEvent::DECREMENT:
        this->decrement_value();
        break;
    default:
        break;
    }
}

int main()
{
    pr_D4.hi();
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    MyHorizontalBarModel my_model = MyHorizontalBarModel(MIN_VALUE, MAX_VALUE, true, 1);

    WidgetHorizontalBar my_horizontal_bar = WidgetHorizontalBar(&my_model, &display, MAX_VALUE, MIN_VALUE, 100, 8, 20, 8);

    display.clear_pixel_buffer();
    display.rect(0, 0, 128, 64);
    display.show();
    pr_D4.lo();

    while (true)
    {
        pr_D5.hi();

        my_model.increment_value();

        my_horizontal_bar.set_value(my_model.get_value());
        my_horizontal_bar.draw_refresh();

        pr_D5.lo();
        sleep_ms(500);
    }

    return 0;
}