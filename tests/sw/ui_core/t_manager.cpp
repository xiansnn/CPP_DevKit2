#include "t_manager.h"


MyManager::MyManager(UIController *_controller)
    : UIModelManager()
{
    make_manager_active();
    update_current_controller(_controller);
}

MyManager::~MyManager()
{
}

/// @brief The event processed by void MyManager for this test are:
/// - LONG_PUSH: if manager is not active, processed by the current_active_model
/// - RELEASED_AFTER_SHORT_TIME: switch activation between the manager and the current model.
/// - INCREMENT: focus to next model
/// - DECREMENT: focus to previous model
/// @param _event
void MyManager::process_control_event(struct_ControlEventData control_event)
{
    switch (control_event.event)
    {
    case UIControlEvent::NONE:
        /* code */
        break;
    case UIControlEvent::LONG_PUSH:
        if (current_active_model != this)
            current_active_model->process_control_event(control_event);
        break;
    case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (current_active_model == this)
            make_managed_model_active();
        else
            make_manager_active();
        break;
    case UIControlEvent::INCREMENT:
        if (current_active_model == this)
        {
            increment_focus();
            this->set_change_flag();
        }
        else
            current_active_model->process_control_event(control_event);
        break;
    case UIControlEvent::DECREMENT:
        if (current_active_model == this)
        {
            decrement_focus();
            this->set_change_flag();
        }
        else
            current_active_model->process_control_event(control_event);
        break;
    default:
        break;
    }
}
