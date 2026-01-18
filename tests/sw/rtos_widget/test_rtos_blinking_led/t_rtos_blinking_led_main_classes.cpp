#include "t_rtos_blinking_led_main_classes.h"

extern rtos_Blinker my_blinker;

myClockController::myClockController(bool is_wrapable)
    : rtos_UIModelManager(is_wrapable)
{
}

myClockController::~myClockController()
{
}

void myClockController::process_control_event(struct_ControlEventData control_event)
{
    if (this->get_rtos_status() == ControlledObjectStatus::IS_IDLE) // wake up focus manager
    {
        if (control_event.event == UIControlEvent::LONG_PUSH)
        {
            this->reset_current_focus_index();
            this->make_managed_rtos_model_active();
            this->managed_rtos_models[0]->process_control_event(control_event);
        }
    }
    else
    {
        if (this->current_active_rtos_model == this)
        {
            switch (control_event.event)
            {
            case UIControlEvent::LONG_PUSH:
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                ((myControlledClockTime *)this->managed_rtos_models[0])->parent_model->update_rtos_status(ControlledObjectStatus::IS_ACTIVE);
                break;
            }
        }
        else
        {
            switch (control_event.event)
            {
            case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
                this->make_rtos_manager_active();
                increment_focus();
                this->make_managed_rtos_model_active();
                ((myControlledClockTime *)this->current_active_rtos_model)->parent_model->notify_all_linked_widget_task();
                break;
            case UIControlEvent::LONG_PUSH:
            case UIControlEvent::TIME_OUT:
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                ((myControlledClockTime *)this->current_active_rtos_model)->parent_model->hour.update_rtos_status(ControlledObjectStatus::IS_WAITING);
                ((myControlledClockTime *)this->current_active_rtos_model)->parent_model->minute.update_rtos_status(ControlledObjectStatus::IS_WAITING);
                ((myControlledClockTime *)this->current_active_rtos_model)->parent_model->second.update_rtos_status(ControlledObjectStatus::IS_WAITING);
                ((myControlledClockTime *)this->current_active_rtos_model)->parent_model->update_rtos_status(ControlledObjectStatus::IS_ACTIVE);

                break;
            default:
                this->forward_control_event_to_active_managed_model(&control_event);
                break;
            }
        }
    }
}

myMainClock::myMainClock()
    : rtos_UIControlledModel(),
      hour("hour", this, 0, 24, 1),
      minute("minute", this, 0, 60, 1),
      second("second", this, 0, 60, 1)
{
    this->update_rtos_status(ControlledObjectStatus::IS_ACTIVE);
    this->hour.update_rtos_status(ControlledObjectStatus::IS_WAITING);
    this->minute.update_rtos_status(ControlledObjectStatus::IS_WAITING);
    this->second.update_rtos_status(ControlledObjectStatus::IS_WAITING);
}

myMainClock::~myMainClock()
{
}

void myMainClock::process_control_event(struct_ControlEventData control_event)
{
    if (this->get_rtos_status() == ControlledObjectStatus::IS_ACTIVE)
    {
        switch (control_event.event)
        {
        case UIControlEvent::LONG_PUSH:
            this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
            break;
        case UIControlEvent::INCREMENT:
            second.increment_value();
            second.notify_all_linked_widget_task(); // if second has a attached widgets
            if (second.get_value() == 0)
            {
                minute.increment_value();
                minute.notify_all_linked_widget_task(); // if minute has a attached widgets
                if (minute.get_value() == 0)
                {
                    hour.increment_value();
                    hour.notify_all_linked_widget_task(); // if hour has a attached widgets
                }
            }
            break;
        default:
            break;
        }
        notify_all_linked_widget_task(); // notify the main clock attached widgets
    }
}

myControlledClockTime::myControlledClockTime(std::string name, myMainClock *parent_model, int min_value, int max_value, int increment)
    : rtos_UIControlledModel(), core_CircularIncremetalControlledModel(increment, min_value, max_value)
{
    this->name = name;
    this->parent_model = parent_model;
    this->update_rtos_status(ControlledObjectStatus::IS_WAITING);
}

myControlledClockTime::~myControlledClockTime()
{
}

void myControlledClockTime::process_control_event(struct_ControlEventData control_event)
{
    switch (control_event.event)
    {
    case UIControlEvent::LONG_PUSH:
        this->parent_model->process_control_event(control_event);
        break;
    case UIControlEvent::INCREMENT:
        this->increment_value();
        this->parent_model->notify_all_linked_widget_task(); // if the controlled clock time has no attached widgets
        this->notify_all_linked_widget_task();               // if the controlled clock time has attached widgets
        break;
    case UIControlEvent::DECREMENT:
        this->decrement_value();
        this->parent_model->notify_all_linked_widget_task(); // if the controlled clock time has no attached widgets
        this->notify_all_linked_widget_task();               // if the controlled clock time has attached widgets
        break;
    default:
        break;
    }
}

rtos_Blinker::rtos_Blinker(uint32_t blink_period_ms)
{
    this->blink_period_ms = blink_period_ms;
}

rtos_Blinker::~rtos_Blinker()
{
}

void rtos_Blinker::add_blinking_widget(rtos_BlinkingWidget *widget)
{
    this->blinking_widgets.insert(widget);
}

void rtos_Blinker::remove_blinking_widget(rtos_BlinkingWidget *widget)
{
    this->blinking_widgets.extract(widget);
}

void rtos_Blinker::refresh_blinking()
{
    this->current_blink_phase = !this->current_blink_phase;
    for (auto &&widget : blinking_widgets)
        widget->blink();
}

rtos_BlinkingWidget::rtos_BlinkingWidget()
{
}

rtos_BlinkingWidget::~rtos_BlinkingWidget()
{
}

void rtos_BlinkingWidget::start_blinking()
{
    this->blinker->add_blinking_widget(this);
}

void rtos_BlinkingWidget::stop_blinking()
{
    restore_canvas_color();
    this->blinker->remove_blinking_widget(this);
}

void rtos_BlinkingWidget::setup_blinking(rtos_Blinker *blinker)
{
    save_canvas_color();
    this->blinker = blinker;
}
