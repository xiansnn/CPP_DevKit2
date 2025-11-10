#include "rtos_ui_core.h"
#include "sw/ui_core/ui_control_event.h"

void rtos_Model::link_widget(rtos_Widget *linked_widget)
{
    this->linked_widget_tasks.insert(linked_widget);
}

void rtos_Model::notify_all_linked_widget_task()
{
    for (auto &&widget : linked_widget_tasks)
    {
        xTaskNotifyGive(widget->task_handle);
    }
}

rtos_Model::rtos_Model(Model *model)
{
    this->model = model;
}

rtos_Model::~rtos_Model()
{
}

void rtos_Widget::send_clear_device_command(QueueHandle_t display_queue, SemaphoreHandle_t sending_done)
{
    struct_DataToShow data_to_display;
    data_to_display.command = DisplayCommand::CLEAR_SCREEN;
    data_to_display.display = this->widget->display_device;
    xQueueSend(display_queue, &data_to_display, portMAX_DELAY); // take 65ms but used fully the CPU
    xSemaphoreTake(sending_done, portMAX_DELAY);
}

rtos_Widget::rtos_Widget(Widget *widget)
{
    this->widget = widget;
}

rtos_Widget::~rtos_Widget()
{
}

rtos_UIController::rtos_UIController(UIController *linked_controller)
{
    this->controller = linked_controller;
    control_event_output_queue = xQueueCreate(5, sizeof(struct_ControlEventData));
}

rtos_UIController::~rtos_UIController()
{
}

rtos_UIControlledModel::rtos_UIControlledModel(UIControlledModel *linked_controlled_model)
    : rtos_Model(linked_controlled_model)
{
}

rtos_UIControlledModel::~rtos_UIControlledModel()
{
}

rtos_UIControlledIncrementalValue::rtos_UIControlledIncrementalValue(UIControlledIncrementalValue *linked_ui_controlled_incremental_value)
    : rtos_UIControlledModel(linked_ui_controlled_incremental_value)
{
}

rtos_UIControlledIncrementalValue::~rtos_UIControlledIncrementalValue()
{
}

rtos_UIModelManager::rtos_UIModelManager(UIModelManager *linked_model_manager)
    : rtos_UIControlledIncrementalValue(linked_model_manager)
{
}

rtos_UIModelManager::~rtos_UIModelManager()
{
}

void rtos_UIModelManager::make_managed_rtos_model_active()
{
    printf("rtos_UIModelManager::make_managed_model_active\n");
    UIModelManager *model = ((UIModelManager *)this->model);
    model->current_active_model = model->managed_models[model->get_value()];
    model->current_active_model->update_status(ControlledObjectStatus::IS_ACTIVE);
    model->update_status(ControlledObjectStatus::IS_WAITING);
}

void rtos_UIModelManager::make_rtos_manager_active()
{
    printf("rtos_UIModelManager::make_rtos_manager_active\n");
    UIModelManager *model = ((UIModelManager *)this->model);
    model->current_active_model->update_status(ControlledObjectStatus::IS_WAITING);
    model->current_active_model = (UIControlledModel *)this;
    model->update_status(ControlledObjectStatus::IS_ACTIVE);
}
