#include "ui_core.h"

UIModelObject::UIModelObject()
{
    last_change_time = time_us_32();
}

UIModelObject::~UIModelObject()
{
    delete current_controller;
}

bool UIModelObject::has_changed()
{
    return this->change_flag;
}

void UIModelObject::clear_change_flag()
{
    this->change_flag = false;
}

uint32_t UIModelObject::get_time_since_last_change()
{
    return time_us_32() - last_change_time;
}

void UIModelObject::update_current_controller(UIController *_new_controller)
{
    if (this->current_controller != _new_controller) // to avoid deadlock with recursive callback
    {
        this->current_controller = _new_controller;
        this->current_controller->update_current_controlled_object(this);
    }
}

void UIModelObject::update_status(ControlledObjectStatus _new_status)
{
    if (this->status != _new_status)
    {
        this->status = _new_status;
        set_change_flag();
    }
}

ControlledObjectStatus UIModelObject::get_status()
{
    return this->status;
}

UIController *UIModelObject::get_current_controller()
{
    return this->current_controller;
}

void UIModelObject::set_change_flag()
{
    last_change_time = time_us_32();
    this->change_flag = true;
}

UIControlledIncrementalValue::UIControlledIncrementalValue(int _min_value, int _max_value, bool _is_wrappable, int _increment)
    : UIModelObject()
{
    this->value = 0;
    this->min_value = _min_value;
    this->max_value = _max_value;
    this->is_wrappable = _is_wrappable;
    this->increment = _increment;
}

UIControlledIncrementalValue::~UIControlledIncrementalValue()
{
}

void UIControlledIncrementalValue::increment_value()
{
    int previous_value = value;
    value += increment;
    if (value > max_value)
        value = (is_wrappable) ? min_value : max_value;
    if (value != previous_value)
        set_change_flag();
}

void UIControlledIncrementalValue::decrement_value()
{
    int previous_value = value;
    value -= increment;
    if (value < min_value)
        value = (is_wrappable) ? max_value : min_value;
    if (value != previous_value)
        set_change_flag();
}

void UIControlledIncrementalValue::set_clipped_value(int _new_value)
{
    int previous_value = value;
    value = std::min(max_value, std::max(min_value, _new_value));
    if (value != previous_value)
        set_change_flag();
}

int UIControlledIncrementalValue::get_value()
{
    return this->value;
}

int UIControlledIncrementalValue::get_min_value()
{
    return min_value;
}

int UIControlledIncrementalValue::get_max_value()
{
    return max_value;
}

UIObjectManager::UIObjectManager(bool is_wrappable)
    : UIControlledIncrementalValue(0, 0, is_wrappable, 1)
{
    update_status(ControlledObjectStatus::IS_ACTIVE);
    current_active_model = this;
}

UIObjectManager::~UIObjectManager()
{
    delete current_active_model;
}

void UIObjectManager::add_managed_model(UIModelObject *_new_model)
{
    this->managed_models.push_back(_new_model);
    this->max_value = managed_models.size() - 1;
}

void UIObjectManager::increment_focus()
{
    int previous_value = value;
    this->increment_value();
    if (value != previous_value) // action takes place only when the value changes
    {
        this->managed_models[previous_value]->update_status(ControlledObjectStatus::IS_WAITING);
        this->managed_models[this->value]->update_status(ControlledObjectStatus::HAS_FOCUS);
    }
}

void UIObjectManager::decrement_focus()
{
    int previous_value = value;
    this->decrement_value();
    if (value != previous_value) // action takes place only when the value changes
    {
        this->managed_models[previous_value]->update_status(ControlledObjectStatus::IS_WAITING);
        this->managed_models[this->value]->update_status(ControlledObjectStatus::HAS_FOCUS);
    }
}

ControlledObjectStatusTimeOutReason UIObjectManager::check_time_out(uint32_t managed_object_status_time_out_us)
{
    ControlledObjectStatusTimeOutReason reason = ControlledObjectStatusTimeOutReason::NO_TIME_OUT;
    if (current_active_model != this) /// - chek time_out for active model
    {
        if (current_active_model->get_time_since_last_change() > managed_object_status_time_out_us)
        {
            get_current_controller()->update_current_controlled_object(this);
            make_manager_active();
            reason = ControlledObjectStatusTimeOutReason::MANAGED_OBJECT_INACTIVE;
        }
    }
    else /// - check time_out for model under focus
    {
        if ((get_time_since_last_change() > managed_object_status_time_out_us) and (managed_models[value]->get_status() == ControlledObjectStatus::HAS_FOCUS))
        {
            managed_models[value]->update_status(ControlledObjectStatus::IS_WAITING);
            reason = ControlledObjectStatusTimeOutReason::MANAGER_INACTIVE;
        }
    }
    return reason;
}

void UIObjectManager::make_managed_object_active()
{
    this->current_active_model = this->managed_models[this->value];
    this->current_active_model->update_status(ControlledObjectStatus::IS_ACTIVE);
    this->update_status(ControlledObjectStatus::IS_WAITING);
}

void UIObjectManager::make_manager_active()
{
    current_active_model->update_status(ControlledObjectStatus::IS_WAITING);
    current_active_model = this;
    this->update_status(ControlledObjectStatus::IS_ACTIVE);
}

UIController::UIController()
{
}

UIController::~UIController()
{
    delete current_controlled_object;
}

void UIController::update_current_controlled_object(UIModelObject *_new_controlled_object)
{
    if (this->current_controlled_object != _new_controlled_object)
    {
        this->current_controlled_object = _new_controlled_object;
        this->current_controlled_object->update_current_controller(this);
    }
}