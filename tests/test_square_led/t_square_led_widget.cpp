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
    MySquareLedModel *actual_displayed_model;

public:
    MyWidgetSquareLed(MySquareLedModel *actual_displayed_model,
                      GraphicDisplayDevice *display_screen,
                      struct_ConfigGraphicFramebuffer graph_cfg,
                      uint8_t widget_anchor_x,
                      uint8_t widget_anchor_y);
    ~MyWidgetSquareLed();
    void draw_refresh();
};

/**
 * @brief Construct a new test square led widget::test square led widget object
 *
 * @param actual_displayed_model
 * @param display_screen
 * @param width
 * @param height
 * @param widget_anchor_x
 * @param widget_anchor_y
 */
MyWidgetSquareLed::MyWidgetSquareLed(MySquareLedModel *actual_displayed_model,
                                     GraphicDisplayDevice *display_screen,
                                     struct_ConfigGraphicFramebuffer graph_cfg,
                                     uint8_t widget_anchor_x,
                                     uint8_t widget_anchor_y)
    : WidgetSquareLed(actual_displayed_model,display_screen, graph_cfg, widget_anchor_x, widget_anchor_y)
{
    this->actual_displayed_model = actual_displayed_model;
}

MyWidgetSquareLed::~MyWidgetSquareLed()
{
}

/**
 * @brief This function implements a special draw_refresh that takes into account the on/off and led_is_blinking status of the model.
 *
 * It insures that the widget consumes processing time only when its on/off status has changed.
 *
 */
void MyWidgetSquareLed::draw_refresh()
{
    assert(this->actual_displayed_model != nullptr);
    /// main step of the function
    /// - first process the status of the displayed object
    this->led_is_blinking = this->actual_displayed_model->blinking_status;
    /// - then widget_blink_refresh() if it is appropriate
    blink_refresh();

    if (this->actual_displayed_model->has_changed()) // check if something changed
    {
        /// check if the model my_bool_value is different from the widget lit_status
        if ((actual_displayed_model->my_bool_value) and (!led_is_on))
        {
            this->light_on();
            rect(0, 0, pixel_memory.frame_width, pixel_memory.frame_height, true, PixelColor::WHITE);
        }
        if ((!actual_displayed_model->my_bool_value) and (led_is_on))
        {
            this->light_off();
            rect(0, 0, pixel_memory.frame_width, pixel_memory.frame_height, true, PixelColor::BLACK);
            draw_border();
        }
        this->display_screen->show(&this->pixel_memory, this->widget_anchor_x, this->widget_anchor_y);
        this->actual_displayed_model->clear_change_flag();
    }
}
