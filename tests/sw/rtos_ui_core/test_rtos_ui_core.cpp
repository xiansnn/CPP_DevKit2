#include "sw/ui_core/rtos_ui_core.h"
#include "sw/widget/widget.h"
#include "sw/display_device/display_device.h"



#include "utilities/probe/probe.h"
Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

#define MAIN_TASK_PERIOD_ms 1000

QueueHandle_t display_queue = xQueueCreate(8, sizeof(struct_DataToShow));
SemaphoreHandle_t data_sent = xSemaphoreCreateBinary(); // synchro between display task and sending task

TaskHandle_t widget_handle_1;
TaskHandle_t widget_handle_2;

void display_gate_keeper_task(void *param)
{
    struct_DataToShow received_data_to_show;

    while (true)
    {
        xQueueReceive(display_queue, &received_data_to_show, portMAX_DELAY);
        switch (received_data_to_show.command)
        {
        case DisplayCommand::SHOW_IMAGE:
            // ((rtos_ST7735 *)received_data_to_show.display)->show_from_display_queue(received_data_to_show);
            tight_loop_contents();
            break;
        case DisplayCommand::CLEAR_SCREEN:
            // ((rtos_ST7735 *)received_data_to_show.display)->clear_device_screen_buffer();
            tight_loop_contents();
            break;
        default:
            break;
        };

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
void main_task(void *pxProbe)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (true)
    {
        for (size_t i = 0; i < 4000; i++)
        {
            ((Probe *)pxProbe)->hi();
            ((Probe *)pxProbe)->lo();
        }
        xTaskNotifyGive(widget_handle_1);
        xTaskNotifyGive(widget_handle_2);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MAIN_TASK_PERIOD_ms));
    }
}
void widget_task_1(void *probe)
{

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        for (size_t i = 0; i < 10000; i++)
        {
            ((Probe *)probe)->hi();
            ((Probe *)probe)->lo();
        }
    }
}
void widget_task_2(void *probe)
{

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        for (size_t i = 0; i < 10000; i++)
        {
            ((Probe *)probe)->hi();
            ((Probe *)probe)->lo();
        }
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(idle_task, "idle_task", 256, &p0, 0, NULL);
    xTaskCreate(main_task, "main_task", 256, &p1, 4, NULL);
    xTaskCreate(widget_task_1, "widget_task_1", 256, &p2, 2, &widget_handle_1);
    xTaskCreate(widget_task_2, "widget_task_2", 256, &p3, 2, &widget_handle_2);

    // xTaskCreate(display_gate_keeper_task, "display_gate_keeper_task", 250, NULL, 6, NULL);

    vTaskStartScheduler();

    while (true)
        tight_loop_contents();

    return 0;
}
