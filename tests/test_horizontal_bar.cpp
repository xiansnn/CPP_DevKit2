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

#include "device_SSD1306/ssd1306.h"
#include "utilities_debug_probe/probe.h"
#include "sw_widget_horizontal_bar/widget_horizontal_bar.h"
#include "sw_ui_core/ui_core.h"


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

class MySimpleHorizontalBarModel : public Model
{
private:
    int value;
    int min_value;
    int max_value;
    bool is_wrappable;
    int increment;

public:
    MySimpleHorizontalBarModel(int _min_value, int _max_value, bool _is_wrappable, int _increment);
    ~MySimpleHorizontalBarModel();
    void increment_value();
    int get_value();
};

class MySimpleHorizontalBarWidget : public WidgetHorizontalBar
{
private:
    /* data */
    void get_value_of_interest();

public:
    MySimpleHorizontalBarWidget(MySimpleHorizontalBarModel *bar_value_model,
                                GraphicDisplayDevice *graphic_display_screen,
                                int max_value, int min_value,
                                struct_ConfigGraphicWidget graph_cfg);
    ~MySimpleHorizontalBarWidget();
};

MySimpleHorizontalBarWidget::MySimpleHorizontalBarWidget(MySimpleHorizontalBarModel *bar_value_model,
                                                         GraphicDisplayDevice *graphic_display_screen,
                                                         int max_value, int min_value,
                                                         struct_ConfigGraphicWidget graph_cfg)
    : WidgetHorizontalBar(bar_value_model,
                          graphic_display_screen,
                          max_value, min_value,
                          graph_cfg)
{
}

MySimpleHorizontalBarWidget::~MySimpleHorizontalBarWidget()
{
}

void MySimpleHorizontalBarWidget::get_value_of_interest()
{
    set_level(((MySimpleHorizontalBarModel *)actual_displayed_model)->get_value());
}

class MyControlledHorizontalBarModel : public UIControlledIncrementalValue
{
private:
    /* data */
public:
    MyControlledHorizontalBarModel(int _min_value, int _max_value, bool _is_wrappable, int _increment);
    ~MyControlledHorizontalBarModel();
    void process_control_event(UIControlEvent _event);
};

class MyControlledHorizontalBarWidget : public WidgetHorizontalBar
{
private:
    /* data */
    void get_value_of_interest();

public:
    MyControlledHorizontalBarWidget(MyControlledHorizontalBarModel *bar_value_model,
                                    GraphicDisplayDevice *graphic_display_screen,
                                    int max_value, int min_value,
                                    struct_ConfigGraphicWidget graph_cfg);
    ~MyControlledHorizontalBarWidget();
};

MyControlledHorizontalBarWidget::MyControlledHorizontalBarWidget(MyControlledHorizontalBarModel *bar_value_model,
                                                                 GraphicDisplayDevice *graphic_display_screen,
                                                                 int max_value, int min_value,
                                                                 struct_ConfigGraphicWidget graph_cfg)
    : WidgetHorizontalBar(bar_value_model,
                          graphic_display_screen,
                          max_value, min_value,
                          graph_cfg)
{
}

MyControlledHorizontalBarWidget::~MyControlledHorizontalBarWidget()
{
}

void MyControlledHorizontalBarWidget::get_value_of_interest()
{
    set_level(((MyControlledHorizontalBarModel *)actual_displayed_model)->get_value());
}

MyControlledHorizontalBarModel::MyControlledHorizontalBarModel(int _min_value, int _max_value, bool _is_wrappable, int _increment)
    : UIControlledIncrementalValue(_min_value, _max_value, _is_wrappable, _increment)
{
}

MyControlledHorizontalBarModel::~MyControlledHorizontalBarModel()
{
}

void MyControlledHorizontalBarModel::process_control_event(UIControlEvent _event)
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

    struct_ConfigGraphicWidget controlled_horizontal_bar_cfg = {
        .pixel_frame_width = 100,
        .pixel_frame_height = 8,
        .fg_color = PixelColor::WHITE,
        .bg_color = PixelColor::BLACK,
        .widget_anchor_x = 20,
        .widget_anchor_y = 8,
        .widget_with_border = true};
    struct_ConfigGraphicWidget simple_horizontal_bar_cfg = {
        .pixel_frame_width = 100,
        .pixel_frame_height = 8,
        .fg_color = PixelColor::WHITE,
        .bg_color = PixelColor::BLACK,
        .widget_anchor_x = 20,
        .widget_anchor_y = 32,
        .widget_with_border = true};

    MyControlledHorizontalBarModel my_model = MyControlledHorizontalBarModel(MIN_VALUE, MAX_VALUE, true, 1);

    MySimpleHorizontalBarModel my_simple_model = MySimpleHorizontalBarModel(MIN_VALUE, MAX_VALUE, true, 1);

    MyControlledHorizontalBarWidget my_horizontal_bar = MyControlledHorizontalBarWidget(&my_model, &display,
                                                                                        MAX_VALUE, MIN_VALUE,
                                                                                        controlled_horizontal_bar_cfg);
    MySimpleHorizontalBarWidget my_simple_horizontal_bar = MySimpleHorizontalBarWidget(&my_simple_model, &display,
                                                                                       MAX_VALUE, MIN_VALUE,
                                                                                       simple_horizontal_bar_cfg);

    display.clear_device_screen_buffer();

    pr_D4.lo();

    while (true)
    {
        pr_D5.hi();

        my_model.increment_value();
        my_model.draw_refresh_all_attached_widgets();

        my_simple_model.increment_value();
        my_simple_model.draw_refresh_all_attached_widgets();

        pr_D5.lo();
        sleep_ms(500);
    }

    return 0;
}

MySimpleHorizontalBarModel::MySimpleHorizontalBarModel(int _min_value, int _max_value, bool _is_wrappable, int _increment)
{
    this->min_value = _min_value;
    this->max_value = _max_value;
    this->is_wrappable = _is_wrappable;
    this->increment = _increment;
    this->value = min_value;
}

MySimpleHorizontalBarModel::~MySimpleHorizontalBarModel()
{
}

void MySimpleHorizontalBarModel::increment_value()
{
    value += increment;
    if (value > max_value)
        value = (is_wrappable) ? min_value : max_value;
    set_change_flag();
}

int MySimpleHorizontalBarModel::get_value()
{
    return value;
}
