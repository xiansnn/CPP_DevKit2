#include "rtos_ui_core.h"

void rtos_Model::update_attached_rtos_widget(rtos_Widget *linked_widget)
{
    this->attached_rtos_widget.insert(linked_widget);
    linked_widget->set_actual_displayed_model(this);
}

void rtos_Model::notify_all_linked_widget_task()
{
    if (!attached_rtos_widget.empty())
    {
        for (auto &&widget : attached_rtos_widget)
        {
            if (widget->task_handle != nullptr)
                xTaskNotifyGive(widget->task_handle);
        }
    }
}

rtos_Model::rtos_Model()
{
}

rtos_Model::~rtos_Model()
{
}

rtos_UIControlledModel::rtos_UIControlledModel()
    : rtos_Model()
{
    this->control_event_input_queue = xQueueCreate(3, sizeof(struct_ControlEventData));
    this->rtos_status = ControlledObjectStatus::IS_IDLE;
}

rtos_UIControlledModel::~rtos_UIControlledModel()
{
}

void rtos_UIControlledModel::update_rtos_status(ControlledObjectStatus _new_status)
{
    if (this->rtos_status != _new_status)
    {
        this->rtos_status = _new_status;
        notify_all_linked_widget_task();
    }
}

ControlledObjectStatus rtos_UIControlledModel::get_rtos_status()
{
    return this->rtos_status;
}

void rtos_UIModelManager::increment_focus()
{
    int previous_focus_index = this->current_focus_index;
    current_focus_index += 1;
    if (current_focus_index > max_focus_index)
        current_focus_index = is_wrapable ? 0 : max_focus_index;
    if (current_focus_index != previous_focus_index)
    {
        managed_rtos_models[current_focus_index]->update_rtos_status(ControlledObjectStatus::HAS_FOCUS);
        notify_all_linked_widget_task();
    }
}

void rtos_UIModelManager::decrement_focus()
{
    int previous_focus_index = this->current_focus_index;
    current_focus_index -= 1;
    if (current_focus_index < 0)
        current_focus_index = is_wrapable ? max_focus_index : 0;
    if (current_focus_index != previous_focus_index)
    {
        managed_rtos_models[current_focus_index]->update_rtos_status(ControlledObjectStatus::HAS_FOCUS);
        notify_all_linked_widget_task();
    }
}

rtos_UIModelManager::rtos_UIModelManager(bool is_wrapable)
    : rtos_UIControlledModel()
{
    this->current_active_rtos_model = this;
    this->is_wrapable = is_wrapable;
    this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
}

rtos_UIModelManager::~rtos_UIModelManager()
{
}

void rtos_UIModelManager::process_event_and_time_out_condition(rtos_UIModelManager *manager, uint time_out_ms)
{
    struct_ControlEventData local_event_data;
    BaseType_t global_timeout_condtion;
    global_timeout_condtion = xQueueReceive(manager->control_event_input_queue,
                                            &local_event_data,
                                            ((manager->get_rtos_status() == ControlledObjectStatus::IS_IDLE)
                                                 ? portMAX_DELAY
                                                 : pdMS_TO_TICKS(time_out_ms))); // switch and encoder timout is replaced by a global timeout

    if (global_timeout_condtion == errQUEUE_EMPTY)
    {
        local_event_data.event = UIControlEvent::TIME_OUT;
        manager->process_control_event(local_event_data);
    }
    else if (local_event_data.event != UIControlEvent::TIME_OUT)
    {
        manager->process_control_event(local_event_data);
    }
}

size_t rtos_UIModelManager::get_current_focus_index()
{
    return current_focus_index;
}

void rtos_UIModelManager::make_managed_rtos_model_active()
{
    this->update_rtos_status(ControlledObjectStatus::IS_WAITING);
    this->current_active_rtos_model = this->managed_rtos_models[this->current_focus_index];
    this->current_active_rtos_model->update_rtos_status(ControlledObjectStatus::IS_ACTIVE);
}

void rtos_UIModelManager::reset_current_focus_index()
{
    this->current_focus_index = 0;
}

void rtos_UIModelManager::make_rtos_manager_active()
{
    current_active_rtos_model->update_rtos_status(ControlledObjectStatus::IS_WAITING);
    current_active_rtos_model = this;
    this->update_rtos_status(ControlledObjectStatus::IS_ACTIVE);
}

void rtos_UIModelManager::add_managed_rtos_model(rtos_UIControlledModel *new_model)
{
    this->managed_rtos_models.push_back(new_model);
    this->max_focus_index = managed_rtos_models.size() - 1;
}

void rtos_UIModelManager::forward_control_event_to_active_managed_model(struct_ControlEventData *control_event)
{
    xQueueSend(current_active_rtos_model->control_event_input_queue, control_event, portMAX_DELAY);
}

core_IncrementControlledModel::core_IncrementControlledModel(int min_value, int max_value, bool is_wrappable, int increment)
{
    this->value = 0;
    this->min_value = min_value;
    this->max_value = max_value;
    this->is_wrappable = is_wrappable;
    this->increment = increment;
}

core_IncrementControlledModel::~core_IncrementControlledModel()
{
}

bool core_IncrementControlledModel::increment_value()
{
    bool changed = false;
    int previous_value = value;
    value += increment;
    if (value > max_value)
        value = (is_wrappable) ? min_value : max_value;
    if (value != previous_value)
        changed = true;

    return changed;
}

bool core_IncrementControlledModel::decrement_value()
{
    bool changed = false;
    int previous_value = value;
    value -= increment;
    if (value < min_value)
        value = (is_wrappable) ? max_value : min_value;
    if (value != previous_value)
        changed = true;

    return changed;
}

bool core_IncrementControlledModel::set_clipped_value(int _new_value)
{
    bool changed = false;
    int previous_value = value;
    value = std::min(max_value, std::max(min_value, _new_value));
    if (value != previous_value)
    {
        changed = true;
    }
    return changed;
}

int core_IncrementControlledModel::get_value()
{
    return this->value;
}

int core_IncrementControlledModel::get_min_value()
{
    return min_value;
}

int core_IncrementControlledModel::get_max_value()
{
    return max_value;
}

core_CircularIncremetalControlledModel::core_CircularIncremetalControlledModel(int increment, int min_value, int max_value)
    : core_IncrementControlledModel(min_value, max_value, true, increment)
{
}

core_CircularIncremetalControlledModel::~core_CircularIncremetalControlledModel()
{
}

bool core_CircularIncremetalControlledModel::increment_value()
{
    bool changed = false;
    int previous_value = value;
    value += increment;

    if (value >= max_value)
        value = min_value;

    if (value != previous_value)
        changed = true;
    return changed;
}

bool core_CircularIncremetalControlledModel::decrement_value()
{
    bool changed = false;
    int previous_value = value;
    value -= increment;

    if (value < min_value)
        value = max_value - increment;

    if (value != previous_value)
        changed = true;
    return changed;
}
