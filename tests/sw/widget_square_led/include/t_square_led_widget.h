#pragma once


#include "sw/widget_square_led/widget_square_led.h"
#include "t_square_led_model.h"

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
                                  struct_ConfigGraphicWidget graph_cfg,
                                  CanvasFormat format);
    ~my_blinking_square_led_widget();
    void get_value_of_interest();
};
