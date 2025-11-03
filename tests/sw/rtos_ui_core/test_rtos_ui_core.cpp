#include "sw/ui_core/ui_core.h"
#include "sw/widget/widget.h"
#include "sw/display_device/display_device.h"

#include "sw/ui_core/rtos_ui_core.h"

#include "t_rtos_ui_core.h"

#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

#define MAIN_TASK_PERIOD_ms 500

QueueHandle_t display_queue = xQueueCreate(8, sizeof(struct_DataToShow));
SemaphoreHandle_t data_sent = xSemaphoreCreateBinary(); // synchro between display task and sending task
//------------------
my_Model my_model = my_Model();
rtos_Model my_rtos_model = rtos_Model(&my_model);
//------------------
my_ProbePrinter probe_widget1 = my_ProbePrinter(&p2);
my_Widget my_widget1 = my_Widget(&probe_widget1, &my_model);
rtos_Widget my_rtos_widget1 = rtos_Widget(&my_widget1);
//------------------
my_ProbePrinter probe_widget2 = my_ProbePrinter(&p3);
my_Widget my_widget2 = my_Widget(&probe_widget2, &my_model);
rtos_Widget my_rtos_widget2 = rtos_Widget(&my_widget2);
//-----------------

void display_gate_keeper_task(void *probe)
{
    struct_DataToShow received_data_to_show;

    while (true)
    {
        xQueueReceive(display_queue, &received_data_to_show, portMAX_DELAY);
        ((Probe*)probe)->hi();
        switch (received_data_to_show.command)
        {
        case DisplayCommand::SHOW_IMAGE:
            ((my_ProbePrinter *)received_data_to_show.display)->show_from_display_queue(received_data_to_show);
            break;
        case DisplayCommand::CLEAR_SCREEN:
             ((my_ProbePrinter *)received_data_to_show.display)->clear_device_screen_buffer();
            break;
        default:
            break;
        };
        ((Probe*)probe)->lo();
        xSemaphoreGive(data_sent);
    }
}

void idle_task(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}
void my_model_task(void *pxProbe)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    my_rtos_model.link_widget(&my_rtos_widget1);
    my_rtos_model.link_widget(&my_rtos_widget2);
    while (true)
    {
        ((my_Model *)my_rtos_model.model)->cyclic_computation((Probe *)pxProbe);
        my_rtos_model.notify_all_linked_widget_task();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MAIN_TASK_PERIOD_ms));
    }
}
void widget_task_1(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // warning: be sure to complete struct_DataToShow before send_image_to_DisplayGateKeeper
        my_rtos_widget1.widget->send_image_to_DisplayGateKeeper(display_queue, data_sent);
    }
}
void widget_task_2(void *probe)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        my_rtos_widget2.widget->send_image_to_DisplayGateKeeper(display_queue, data_sent);
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(idle_task, "idle_task", 256, &p0, 0, NULL);
    xTaskCreate(my_model_task, "main_task", 256, &p1, 4, NULL);
    xTaskCreate(widget_task_1, "widget_task_1", 256, NULL, 2, &my_rtos_widget1.task_handle);
    xTaskCreate(widget_task_2, "widget_task_2", 256, NULL, 2, &my_rtos_widget2.task_handle);

    xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 256, &p4, 6, NULL);

    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
