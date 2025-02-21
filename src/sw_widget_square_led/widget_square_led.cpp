#include "widget_square_led.h"

WidgetSquareLed::WidgetSquareLed(UIModelObject *actual_displayed_model,
                                 GraphicDisplayDevice *display_screen,
                                 struct_ConfigGraphicFramebuffer graph_cfg,
                                 uint8_t widget_anchor_x,
                                 uint8_t widget_anchor_y,
                                 bool widget_with_border)
    : GraphicWidget(display_screen, actual_displayed_model, graph_cfg, widget_anchor_x, widget_anchor_y, widget_with_border)
{
}

WidgetSquareLed::~WidgetSquareLed()
{
}

void WidgetSquareLed::blink_refresh()
{
    if (this->led_is_blinking and this->blinking_phase_has_changed())
    {
        led_is_on = !led_is_on;
        draw_led();
    }
}

void WidgetSquareLed::draw_led()
{
    PixelColor color = (this->led_is_on) ? PixelColor::WHITE : PixelColor::BLACK;
    switch (color)
    {
    case PixelColor::WHITE:
        rect(widget_start_x, widget_start_y, widget_width, widget_height, true, color);
        break;
    case PixelColor::BLACK:
        rect(widget_start_x, widget_start_y, widget_width, widget_height, true, PixelColor::BLACK);
        draw_border();
        break;

    default:
        break;
    }
    show();
}

void WidgetSquareLed::blink_off()
{
    this->led_is_blinking = false;
}

void WidgetSquareLed::blink_on()
{
    this->led_is_blinking = true;
}
void WidgetSquareLed::light_on()
{
    this->led_is_on = true;
}

void WidgetSquareLed::light_off()
{
    this->led_is_on = false;
}
