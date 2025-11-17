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

rtos_Model::rtos_Model()
{
}

rtos_Model::~rtos_Model()
{
}

void rtos_Widget::send_clear_device_command(QueueHandle_t display_queue, SemaphoreHandle_t sending_done)
{
    struct_DataToShow data_to_display;
    data_to_display.command = DisplayCommand::CLEAR_SCREEN;
    // data_to_display.display = this->widget->display_device;
    xQueueSend(display_queue, &data_to_display, portMAX_DELAY);
    xSemaphoreTake(sending_done, portMAX_DELAY);
}

rtos_Widget::rtos_Widget()
{
}

rtos_Widget::~rtos_Widget()
{
}

rtos_UIControlledModel::rtos_UIControlledModel()
    : rtos_Model()
{
    this->control_event_input_queue = xQueueCreate(3, sizeof(struct_ControlEventData));
}

rtos_UIControlledModel::~rtos_UIControlledModel()
{
}

rtos_UIModelManager::rtos_UIModelManager()
    : rtos_UIControlledModel()
{
}

rtos_UIModelManager::~rtos_UIModelManager()
{
}
