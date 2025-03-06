#include "widget_square_led.h"

WidgetBlinkingSquareLed::WidgetBlinkingSquareLed(Model *actual_displayed_model,
                                                 GraphicDisplayDevice *display_screen,
                                                 struct_ConfigGraphicWidget graph_cfg)
    : WidgetSquareLed(actual_displayed_model,display_screen, graph_cfg), Blinker()
{
}

WidgetBlinkingSquareLed::~WidgetBlinkingSquareLed()
{
}

void WidgetBlinkingSquareLed::draw()
{
    get_value_of_interest();
    //==============process blinking
    compute_blinking_phase();

    if (has_blinking_changed() or actual_displayed_model->has_changed())
    {
        clear_pixel_buffer();
        switch (led_status)
        {
        case LEDStatus::LED_IS_BLINKING:
            led_is_on = !led_is_on;
            if (led_is_on)
                rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, true, fg_color);
            else
                draw_border();
            clear_blinking_phase_change();
            show();
            break;
        case LEDStatus::LED_IS_OFF:
            led_is_on = false;
            draw_border();
            show();
            actual_displayed_model->draw_widget_done();
            break;
        case LEDStatus::LED_IS_ON:
            led_is_on = true;
            rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, true, fg_color);
            show();
            actual_displayed_model->draw_widget_done();
            break;
        default:
            break;
        }
        show();
    }

    // if ((led_status == LEDStatus::LED_IS_BLINKING) and (has_blinking_changed()))
    // {
    //     clear_pixel_buffer();
    //     led_is_on = !led_is_on;
    //     if (led_is_on)
    //         rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, true, fg_color);
    //     else
    //         draw_border();
    //     show();
    //     clear_blinking_phase_change();
    // }
    //============== normal job
    // else
    // {
    // if (actual_displayed_model->has_changed())
    // {
    //     clear_pixel_buffer();
    //     //==============draw widget
    //     if (led_status == LEDStatus::LED_IS_ON)
    //     {
    //         led_is_on = true;
    //         rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, true, fg_color);
    //     }
    //     else
    //     {
    //         led_is_on = false;
    //         draw_border();
    //     }
    //     //=============== show and acknowledge change_flag
    //     show();
    //     actual_displayed_model->draw_widget_done();
    // }
    // }
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
