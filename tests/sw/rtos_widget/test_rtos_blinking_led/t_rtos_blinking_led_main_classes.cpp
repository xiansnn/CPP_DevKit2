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
    if (this->get_rtos_status() == ControlledObjectStatus::IS_IDLE) // wake up focus manager
    {
        if (control_event.event == UIControlEvent::LONG_PUSH)
        {
            this->reset_current_focus_index();
            this->managed_rtos_models[0]->process_control_event(control_event);
            this->make_managed_rtos_model_active();
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
                this->make_managed_rtos_model_active();
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
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                this->forward_control_event_to_active_managed_model(&control_event);
                break;
            case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
                this->make_rtos_manager_active();
                increment_focus();
                this->make_managed_rtos_model_active();
                break;
            case UIControlEvent::TIME_OUT:
            printf("myFocusManager as not active : timeout\n");
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
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
    this->hour.update_rtos_status(ControlledObjectStatus::IS_IDLE);
    this->minute.update_rtos_status(ControlledObjectStatus::IS_IDLE);
    this->second.update_rtos_status(ControlledObjectStatus::IS_IDLE);
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
            second.notify_all_linked_widget_task();
            if (second.get_value() == 0)
            {
                minute.increment_value();
                minute.notify_all_linked_widget_task();
                if (minute.get_value() == 0)
                {
                    hour.increment_value();
                    hour.notify_all_linked_widget_task();
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
            this->hour.update_rtos_status(ControlledObjectStatus::IS_IDLE);
            this->minute.update_rtos_status(ControlledObjectStatus::IS_IDLE);
            this->second.update_rtos_status(ControlledObjectStatus::IS_IDLE);
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
        this->update_rtos_status(ControlledObjectStatus::HAS_FOCUS);
        this->parent_model->process_control_event(control_event);
        break;
    case UIControlEvent::INCREMENT:
        this->increment_value();
        this->notify_all_linked_widget_task();
        break;
    case UIControlEvent::DECREMENT:
        this->decrement_value();
        this->notify_all_linked_widget_task();
        break;
    default:
        break;
    }
}
