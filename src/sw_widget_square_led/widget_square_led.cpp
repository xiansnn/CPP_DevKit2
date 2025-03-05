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
    update_blink_phase_change();
    get_value_of_interest();
    if ((led_status == LEDStatus::LED_IS_BLINKING) and (blink_triggered))
    {
        clear_pixel_buffer();
        led_is_on = !led_is_on;
        if (led_is_on)
        rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, true, fg_color);
        else
        draw_border();
        show();
        blink_triggered = false;
    }
    else
    {
        if (actual_displayed_model->has_changed())
        {
            clear_pixel_buffer();
            if (led_status == LEDStatus::LED_IS_ON)
            {
                led_is_on = true;
                rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, true, fg_color);
            }
            else
            {
                led_is_on = false;
                draw_border();
            }
            show();
            actual_displayed_model->draw_widget_done();
        }
    }
}

WidgetSquareLed::WidgetSquareLed(Model *actual_displayed_model,
                                 GraphicDisplayDevice *graphic_display_screen,
                                 struct_ConfigGraphicWidget graph_cfg)
    : GraphicWidget(graphic_display_screen, graph_cfg, actual_displayed_model)
{
}

WidgetSquareLed::~WidgetSquareLed()
{
}

void WidgetSquareLed::draw()
{
    if (actual_displayed_model->has_changed())
    {
        clear_pixel_buffer();
        get_value_of_interest();
        if (led_is_on)
            rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, true, fg_color);
        else
            draw_border();
        show();
        actual_displayed_model->draw_widget_done();
    }
}
