#include "widget_square_led.h"

WidgetBlinkingSquareLed::WidgetBlinkingSquareLed(Model *actual_displayed_model,
                                                 GraphicDisplayDevice *display_screen,
                                                 struct_ConfigGraphicWidget graph_cfg)
    : GraphicWidget(display_screen, graph_cfg, actual_displayed_model), Blinker()
{
}

WidgetBlinkingSquareLed::~WidgetBlinkingSquareLed()
{
}

void WidgetBlinkingSquareLed::blink_refresh()
{
    if ((led_status == LEDStatus::LED_IS_BLINKING) and (this->blinking_phase_has_changed()))
    {
        actual_displayed_model->set_change_flag();
    }
}

void WidgetBlinkingSquareLed::draw()
{
    blink_refresh();
    if (actual_displayed_model->has_changed())
    {
        clear_pixel_buffer();
        get_value_of_interest();
        switch (led_status)
        {
        case LEDStatus::LED_IS_BLINKING:
            led_is_on = !led_is_on;
            break;
        case LEDStatus::LED_IS_ON:
            led_is_on = true;
            break;
        case LEDStatus::LED_IS_OFF:
            led_is_on = false;
            break;

        default:
            break;
        }
        if (led_is_on)
            rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, true, fg_color);
        else
            draw_border();
        // rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, false, fg_color);

        show();
        actual_displayed_model->ack_widget_drawn();
    }
}
