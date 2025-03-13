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

#include "sw/widget_square_led.h"
#include "t_square_led_model.cpp"

/**
 * @brief test_square_led_widget : Example of final implementation of w_SquareLed
 *
 */
class my_blinking_square_led_widget : public WidgetBlinkingSquareLed
{
private:
public:
    my_blinking_square_led_widget(MySquareLedModel *actual_displayed_model,
                                  GraphicDisplayDevice *graphic_display_screen,
                                  struct_ConfigGraphicWidget graph_cfg);
    ~my_blinking_square_led_widget();
    void get_value_of_interest();
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
my_blinking_square_led_widget::my_blinking_square_led_widget(MySquareLedModel *actual_displayed_model,
                                                             GraphicDisplayDevice *graphic_display_screen,
                                                             struct_ConfigGraphicWidget graph_cfg)
    : WidgetBlinkingSquareLed(actual_displayed_model, graphic_display_screen, graph_cfg)
{
}

my_blinking_square_led_widget::~my_blinking_square_led_widget()
{
}

void my_blinking_square_led_widget::get_value_of_interest()
{
    ControlledObjectStatus model_status = ((MySquareLedModel *)this->actual_displayed_model)->get_status();

    switch (model_status)
    {
    case ControlledObjectStatus::IS_ACTIVE:
        this->led_status = LEDStatus::LED_IS_BLINKING;
        break;
    case ControlledObjectStatus::IS_WAITING:
        this->led_status = LEDStatus::LED_IS_OFF;
        break;
    case ControlledObjectStatus::HAS_FOCUS:
        this->led_status = LEDStatus::LED_IS_ON;
        break;

    default:
        break;
    }
}
