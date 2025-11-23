#include "rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"

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
    xQueueSend(display_queue, &data_to_display, portMAX_DELAY);
    xSemaphoreTake(sending_done, portMAX_DELAY);
}

rtos_Widget::rtos_Widget(rtos_Model *actual_displayed_model, DisplayDevice *display_device)
{
    this->display_device = display_device;
    this->data_to_display.display = this->display_device;
    if (actual_displayed_model != nullptr)
    {
        this->actual_displayed_model = actual_displayed_model;
        this->actual_displayed_model->update_attached_rtos_widget(this);
        // actual_displayed_model->notify_all_linked_widget_task();
    }
}

rtos_Widget::~rtos_Widget()
{
}

void rtos_Widget::set_display_device(DisplayDevice *_new_display_device)
{
    this->display_device = _new_display_device;
}

rtos_PrintWidget::rtos_PrintWidget(PrinterDevice *display_device, rtos_Model *actual_displayed_model)
    : rtos_Widget(actual_displayed_model, display_device)
{
}

rtos_PrintWidget::~rtos_PrintWidget()
{
}
