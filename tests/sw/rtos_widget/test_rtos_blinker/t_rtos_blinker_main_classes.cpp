#include "t_rtos_blinker_main_classes.h"
#include "t_rtos_blinker_config.h"

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
            this->current_active_rtos_model->process_control_event(control_event);
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
                ((myControlledClockTime *)this->current_active_rtos_model)->parent_model->hour->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                ((myControlledClockTime *)this->current_active_rtos_model)->parent_model->minute->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                ((myControlledClockTime *)this->current_active_rtos_model)->parent_model->second->update_rtos_status(ControlledObjectStatus::IS_IDLE);
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
    : rtos_UIControlledModel()
{
    this->hour = new myControlledClockTime("hour", this, 0, 24, 1);
    this->minute = new myControlledClockTime("minute", this, 0, 60, 1);
    this->second = new myControlledClockTime("second", this, 0, 60, 1);

    this->update_rtos_status(ControlledObjectStatus::IS_ACTIVE);
    this->hour->update_rtos_status(ControlledObjectStatus::IS_IDLE);
    this->minute->update_rtos_status(ControlledObjectStatus::IS_IDLE);
    this->second->update_rtos_status(ControlledObjectStatus::IS_IDLE);
}

myMainClock::~myMainClock()
{
    delete hour;
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
            second->increment_value();
            if (second->get_value() == 0)
            {
                minute->increment_value();
                if (minute->get_value() == 0)
                {
                    hour->increment_value();
                }
            }
            break;
        default:
            break;
        }
    }
    notify_all_linked_widget_task();
}

myControlledClockTime::myControlledClockTime(std::string name, myMainClock *parent_model, int min_value, int max_value, int increment)
    : rtos_UIControlledModel(), core_CircularIncremetalControlledModel(increment, min_value, max_value)
{
    this->name = name;
    this->parent_model = parent_model;
    this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
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
        this->parent_model->notify_all_linked_widget_task();
        break;
    case UIControlEvent::DECREMENT:
        this->decrement_value();
        this->parent_model->notify_all_linked_widget_task();
        break;
    default:
        break;
    }
}