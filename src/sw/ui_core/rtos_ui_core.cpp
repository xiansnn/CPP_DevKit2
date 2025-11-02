#include "rtos_ui_core.h"

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

void rtos_Widget::send_image_to_DisplayGateKeeper(QueueHandle_t display_queue, SemaphoreHandle_t sending_done)
{

    this->data_to_display.command = DisplayCommand::SHOW_IMAGE;
    xQueueSend(display_queue, &(this->data_to_display), portMAX_DELAY); // take 65ms but used fully the CPU
    xSemaphoreTake(sending_done, portMAX_DELAY);
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
    this->data_to_display.display = this->widget->display_device;
}

rtos_Widget::~rtos_Widget()
{
}