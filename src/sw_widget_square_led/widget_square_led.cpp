#include "widget_square_led.h"

WidgetSquareLed::WidgetSquareLed(DisplayDevice *display_screen,
                         size_t frame_width, size_t frame_height,
                         uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                         bool widget_with_border,
                         FramebufferColor color, FramebufferFormat framebuffer_format)
    : Widget(display_screen, frame_width, frame_height, widget_anchor_x, widget_anchor_y, widget_with_border, 1, framebuffer_format)
{
}

WidgetSquareLed::~WidgetSquareLed()
{
    delete display_screen;
}

void WidgetSquareLed::light_on()
{
    this->led_is_on = true;
}

void WidgetSquareLed::light_off()
{
    this->led_is_on = false;
}

void WidgetSquareLed::blink_refresh()
{
    if (this->led_is_blinking and this->blinking_phase_has_changed())
    {
        if (!led_is_on)
        {
            this->light_on();
            rect(widget_start_x, widget_start_y, widget_width, widget_height, true, FramebufferColor::WHITE);
            this->display_screen->show(this, this->widget_anchor_x, this->widget_anchor_y);
        }
        else
        {
            this->light_off();
            draw_border();
            rect(widget_start_x, widget_start_y, widget_width, widget_height, true, FramebufferColor::BLACK);
            this->display_screen->show(this, this->widget_anchor_x, this->widget_anchor_y);
        }
    }
}

void WidgetSquareLed::blink_off()
{
    this->led_is_blinking = false;
}

void WidgetSquareLed::blink_on()
{
    this->led_is_blinking = true;
}
