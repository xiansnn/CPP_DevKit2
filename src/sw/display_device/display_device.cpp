
#include "display_device.h"

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
    xSemaphoreTake(this->display_device_mutex,portMAX_DELAY);
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
