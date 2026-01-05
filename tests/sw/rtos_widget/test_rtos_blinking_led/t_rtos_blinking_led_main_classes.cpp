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
        // printf("focus_manager: WAKE_UP\n");
        this->make_managed_rtos_model_active();
    }
    else
    {
        if (this->current_active_rtos_model == this)
        {
            switch (control_event.event)
            {
            case UIControlEvent::LONG_PUSH:
                printf("focus_manager: LONG_PUSH\n");
                this->current_active_rtos_model->process_control_event(control_event);
                break;
            case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
                // printf("focus_manager focus on [%d]: RELEASED_AFTER_SHORT_TIME\n", get_current_focus_index());
                this->make_managed_rtos_model_active();
                this->forward_control_event_to_active_managed_model(&control_event);
                break;
            case UIControlEvent::INCREMENT:
                this->increment_focus();
                // printf("focus_manager focus on [%d]: INCREMENT\n", get_current_focus_index());
                break;
            case UIControlEvent::DECREMENT:
                this->decrement_focus();
                // printf("focus_manager focus on [%d]: DECREMENT\n", get_current_focus_index());
                break;
            case UIControlEvent::TIME_OUT:
                // printf("focus_manager focus on [%d]: TIME_OUT\n", get_current_focus_index());
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                this->current_active_rtos_model->process_control_event(control_event);
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
                // printf("make_rtos_manager_active(): RELEASED_AFTER_SHORT_TIME\n");
                this->make_rtos_manager_active();
                break;
            case UIControlEvent::TIME_OUT:
                // printf("focus_manager: TIME_OUT\n");
                this->current_active_rtos_model->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                this->current_active_rtos_model = this;
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
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
            // printf("main_clock:\tINCREMENT second to %d\n", second.get_value());
            // second.notify_all_linked_widget_task();
            if (second.get_value() == 0)
            {
                minute.increment_value();
                // printf("main_clock:\t\t\tINCREMENT minute to %d\n", minute.get_value());
                // minute.notify_all_linked_widget_task();
                if (minute.get_value() == 0)
                {
                    hour.increment_value();
                    // printf("main_clock:\t\t\t\t\t\tINCREMENT hour to %d\n", hour.get_value());
                    // hour.notify_all_linked_widget_task();
                }
            }
            break;
        case UIControlEvent::TIME_OUT:
            this->update_rtos_status(ControlledObjectStatus::IS_ACTIVE);
            break;
        default:
            break;
        }
        notify_all_linked_widget_task();
    }
    else
    {
        switch (control_event.event)
        {
        case UIControlEvent::LONG_PUSH:
            this->update_rtos_status(ControlledObjectStatus::IS_ACTIVE);
            break;
        case UIControlEvent::TIME_OUT:
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
        printf("managed_clock:\tINCREMENT %s to %d\n", name.c_str(), get_value());
        this->notify_all_linked_widget_task();
        break;
    case UIControlEvent::DECREMENT:
        this->decrement_value();
        printf("managed_clock:\tDECREMENT %s to %d\n", name.c_str(), get_value());
        this->notify_all_linked_widget_task();
        break;
    case UIControlEvent::TIME_OUT:
        printf("managed_clock:\t\tTIME_OUT %s \n", name.c_str());
        this->parent_model->process_control_event(control_event);
        break;
    default:
        break;
    }
}
