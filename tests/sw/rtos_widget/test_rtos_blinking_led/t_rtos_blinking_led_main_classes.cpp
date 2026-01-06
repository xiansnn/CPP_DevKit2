#include "t_rtos_blinking_led_main_classes.h"

myFocusManager::myFocusManager(bool is_wrapable)
    : rtos_UIModelManager(is_wrapable)
{
}

myFocusManager::~myFocusManager()
{
}

void myFocusManager::process_control_event(struct_ControlEventData control_event)
{
    if (this->get_rtos_status() == ControlledObjectStatus::IS_IDLE)
    {
        if (control_event.event == UIControlEvent::LONG_PUSH)
        {
            this->managed_rtos_models[0]->process_control_event(control_event);
            this->make_rtos_manager_active();
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
            case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
                increment_focus();
                this->make_managed_rtos_model_active();
                break;
            case UIControlEvent::TIME_OUT:
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                ((myControlledClockTime *)this->managed_rtos_models[0])->parent_model->update_rtos_status(ControlledObjectStatus::IS_ACTIVE);
                break;
            default:
                break;
            }
        }
        else
        {
            switch (control_event.event)
            {
            case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
                this->make_rtos_manager_active();
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
      hour("hour", this, 0, 24, 3),
      minute("minute", this, 0, 60, 15),
      second("second", this, 0, 60, 20)
{
    this->rtos_status = ControlledObjectStatus::IS_ACTIVE;
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
            if (second.get_value() == 0)
            {
                minute.increment_value();
                if (minute.get_value() == 0)
                {
                    hour.increment_value();
                }
            }
            notify_all_linked_widget_task();
            break;
        default:
            break;
        }
    }
    else
    {
        switch (control_event.event)
        {
        case UIControlEvent::LONG_PUSH:
            this->update_rtos_status(ControlledObjectStatus::IS_ACTIVE);
            break;
        default:
            break;
        }
    }
}

myControlledClockTime::myControlledClockTime(std::string name, myMainClock *parent_model, int min_value, int max_value, int increment)
    : rtos_UIControlledModel(), core_CircularIncremetalControlledModel(increment, min_value, max_value)
{
    this->name = name;
    this->parent_model = parent_model;
}

myControlledClockTime::~myControlledClockTime()
{
}

void myControlledClockTime::process_control_event(struct_ControlEventData control_event)
{
    switch (control_event.event)
    {
    case UIControlEvent::LONG_PUSH:
        printf("managed_clock:\tLONG_PUSH (during %s) \n", name.c_str());
        this->parent_model->process_control_event(control_event);
        break;
    case UIControlEvent::INCREMENT:
        this->increment_value();
        printf("managed_clock(%s):\tINCREMENT to %d\n", name.c_str(), get_value());
        this->notify_all_linked_widget_task();
        break;
    case UIControlEvent::DECREMENT:
        this->decrement_value();
        printf("managed_clock(%s):\tINCREMENT to %d\n", name.c_str(), get_value());
        this->notify_all_linked_widget_task();
        break;
    default:
        break;
    }
}
