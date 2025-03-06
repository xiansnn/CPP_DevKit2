#include "widget_square_led.h"

WidgetBlinkingSquareLed::WidgetBlinkingSquareLed(Model *actual_displayed_model,
                                                 GraphicDisplayDevice *display_screen,
                                                 struct_ConfigGraphicWidget graph_cfg)
    : WidgetSquareLed(actual_displayed_model, display_screen, graph_cfg), Blinker()
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

    if ((led_status == LEDStatus::LED_IS_BLINKING) and has_blinking_changed())
    {
        clear_pixel_buffer();
        led_is_on = !led_is_on;
        if (led_is_on)
            rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, true, fg_color);
        else
            draw_border();
        clear_blinking_phase_change();
        show();
    }
    //================ draw widget
    else
    {
        if (actual_displayed_model->has_changed())
        {
            clear_pixel_buffer();
            switch (led_status)
            {
            case LEDStatus::LED_IS_OFF:
                led_is_on = false;
                draw_border();
                break;
            case LEDStatus::LED_IS_ON:
                led_is_on = true;
                rect(0, 0, pixel_frame.pixel_frame_width, pixel_frame.pixel_frame_height, true, fg_color);
                break;
            default:
                break;
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

WidgetFocusIndicator::WidgetFocusIndicator(Model *actual_displayed_model,
                                           GraphicDisplayDevice *graphic_display_screen,
                                           struct_ConfigGraphicWidget graph_cfg)
    : WidgetBlinkingSquareLed(actual_displayed_model, graphic_display_screen, graph_cfg)
{
}

WidgetFocusIndicator::~WidgetFocusIndicator()
{
}

void WidgetFocusIndicator::get_value_of_interest()
{
    ControlledObjectStatus model_status = ((UIControlledModel *)this->actual_displayed_model)->get_status();

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
