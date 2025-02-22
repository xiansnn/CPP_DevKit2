/**
 * @file t_square_led_widget.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "widget_square_led.h"
#include "t_square_led_model.cpp"

/**
 * @brief test_square_led_widget : Example of final implementation of w_SquareLed
 *
 */
class MyWidgetSquareLed : public WidgetSquareLed
{
private:
public:
    MyWidgetSquareLed(MySquareLedModel *actual_displayed_model,
                      GraphicDisplayDevice *graphic_display_screen,
                      struct_ConfigGraphicFramebuffer graph_cfg,
                      uint8_t widget_anchor_x,
                      uint8_t widget_anchor_y);
    ~MyWidgetSquareLed();
    void draw_refresh();
    void draw();
};

/**
 * @brief Construct a new test square led widget::test square led widget object
 *
 * @param actual_displayed_model
 * @param graphic_display_screen
 * @param width
 * @param height
 * @param widget_anchor_x
 * @param widget_anchor_y
 */
MyWidgetSquareLed::MyWidgetSquareLed(MySquareLedModel *actual_displayed_model,
                                     GraphicDisplayDevice *graphic_display_screen,
                                     struct_ConfigGraphicFramebuffer graph_cfg,
                                     uint8_t widget_anchor_x,
                                     uint8_t widget_anchor_y)
    : WidgetSquareLed(actual_displayed_model, graphic_display_screen, graph_cfg, widget_anchor_x, widget_anchor_y)
{
}

MyWidgetSquareLed::~MyWidgetSquareLed()
{
}

void MyWidgetSquareLed::draw()
{
    this->led_is_on = ((MySquareLedModel *)this->actual_displayed_model)->my_bool_value;
    draw_led();
}

void MyWidgetSquareLed::draw_refresh()
{
    this->led_is_blinking = ((MySquareLedModel *)this->actual_displayed_model)->blinking_status;
    WidgetSquareLed::blink_refresh();
    GraphicWidget::draw_refresh();
}
