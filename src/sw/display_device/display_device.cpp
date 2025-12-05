
#include "display_device.h"
#include "sw/widget/rtos_widget.h"

GraphicDisplayDevice::GraphicDisplayDevice(size_t screen_width,
                                           size_t screen_height)
{
    this->TFT_panel_height_in_pixel = screen_height;
    this->TFT_panel_width_in_pixel = screen_width;
}

GraphicDisplayDevice::~GraphicDisplayDevice()
{
}

TerminalConsole::TerminalConsole(size_t number_of_char_width, size_t number_of_char_height)
{
    this->number_of_line = number_of_char_height;
    this->number_of_column = number_of_char_width;
    this->text_buffer_size = number_of_char_width * number_of_char_height + 1; // +1 because this is a C-style char[] ended with \x0

    this->text_buffer = new char[text_buffer_size];
}

TerminalConsole::~TerminalConsole()
{
    delete[] this->text_buffer;
}

void TerminalConsole::show()
{
    stdio_printf(this->text_buffer);
}

DisplayDevice::DisplayDevice()
{
}

DisplayDevice::~DisplayDevice()
{
}

rtos_DisplayDevice::rtos_DisplayDevice()
{
    this->display_device_mutex = xSemaphoreCreateMutex();
}

rtos_DisplayDevice::~rtos_DisplayDevice()
{
}

rtos_GraphicDisplayDevice::rtos_GraphicDisplayDevice()
    : rtos_DisplayDevice()
{
    this->input_queue = xQueueCreate(5, sizeof(struct_DataToShow));
}

rtos_GraphicDisplayDevice::~rtos_GraphicDisplayDevice()
{
}

void rtos_TerminalConsole::show_from_display_queue(char *text_to_print)
{
    xSemaphoreTake(this->display_device_mutex, portMAX_DELAY);
    stdio_printf(text_to_print);
    xSemaphoreGive(this->display_device_mutex);
}

rtos_TerminalConsole::rtos_TerminalConsole()
    : rtos_DisplayDevice()
{
    this->input_queue = xQueueCreate(5, sizeof(char *));
}

rtos_TerminalConsole::~rtos_TerminalConsole()
{
}

rtos_GraphicDisplayGateKeeper::rtos_GraphicDisplayGateKeeper()
{
    this->graphic_widget_data = xQueueCreate(8, sizeof(struct_WidgetDataToGateKeeper));
    this->data_sent = xSemaphoreCreateBinary();
}

rtos_GraphicDisplayGateKeeper::~rtos_GraphicDisplayGateKeeper()
{
}

void rtos_GraphicDisplayGateKeeper::send_clear_device_command(rtos_GraphicDisplayDevice *device)
{
    struct_WidgetDataToGateKeeper data_to_display;
    data_to_display.command = DisplayCommand::CLEAR_SCREEN;
    data_to_display.display = device;
    xQueueSend(graphic_widget_data, &data_to_display, portMAX_DELAY); // take 65ms but used fully the CPU
    xSemaphoreTake(data_sent, portMAX_DELAY);
}

void rtos_GraphicDisplayGateKeeper::send_widget_data(rtos_GraphicWidget *widget)
{
    widget->widget_data_to_gatekeeper.command = DisplayCommand::SHOW_IMAGE;
    xQueueSend(graphic_widget_data, &widget->widget_data_to_gatekeeper, portMAX_DELAY); // take 65ms but used fully the CPU
    xSemaphoreTake(data_sent, portMAX_DELAY);
}

void rtos_GraphicDisplayGateKeeper::receive_widget_data(struct_WidgetDataToGateKeeper received_widget_data)
{
    switch (received_widget_data.command)
    {
    case DisplayCommand::SHOW_IMAGE:
        ((rtos_GraphicDisplayDevice *)received_widget_data.display)->show_widget(received_widget_data.widget);
        break;
    case DisplayCommand::CLEAR_SCREEN:
        ((rtos_GraphicDisplayDevice *)received_widget_data.display)->clear_device_screen_buffer();
        break;
    default:
        break;
    }
    xSemaphoreGive(data_sent);
}
