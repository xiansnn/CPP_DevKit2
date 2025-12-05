#include "rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"

void rtos_GraphicWidget::send_image_to_DisplayGateKeeper(QueueHandle_t display_queue, SemaphoreHandle_t sending_done)
{
    this->widget_data_to_gatekeeper.command = DisplayCommand::SHOW_IMAGE;
    xQueueSend(display_queue, &(this->widget_data_to_gatekeeper), portMAX_DELAY); // take 65ms but used fully the CPU
    xSemaphoreTake(sending_done, portMAX_DELAY);
}

void rtos_GraphicWidget::send_clear_device_command(QueueHandle_t display_queue, SemaphoreHandle_t sending_done)
{
    struct_DataToShow data_to_display;
    data_to_display.command = DisplayCommand::CLEAR_SCREEN;
    xQueueSend(display_queue, &data_to_display, portMAX_DELAY);
    xSemaphoreTake(sending_done, portMAX_DELAY);
}

rtos_Widget::rtos_Widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device)
{
    this->display_device = display_device;
    this->widget_data_to_gatekeeper.display = this->display_device;

    if (actual_displayed_model != nullptr)
    {
        this->actual_rtos_displayed_model = actual_displayed_model;
        this->actual_rtos_displayed_model->update_attached_rtos_widget(this);
    }
}

rtos_Widget::~rtos_Widget()
{
}

void rtos_Widget::set_display_device(rtos_DisplayDevice *_new_display_device)
{
    this->display_device = _new_display_device;
    this->widget_data_to_gatekeeper.display = this->display_device;
}

void rtos_Widget::update_widget_anchor(uint8_t x, uint8_t y)
{
    this->widget_anchor_x = x;
    this->widget_anchor_y = y;
}

void rtos_Widget::add_widget(rtos_Widget *_sub_widget)
{
    this->rtos_widgets.push_back(_sub_widget);
}

rtos_GraphicWidget::rtos_GraphicWidget(rtos_Model *actual_displayed_model,
                                       struct_ConfigGraphicWidget graph_cfg, CanvasFormat canvas_format,
                                       rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
    this->widget_anchor_x = graph_cfg.widget_anchor_x;
    this->widget_anchor_y = graph_cfg.widget_anchor_y;
    this->widget_data_to_gatekeeper.widget = this;

    drawer = new GraphicDrawer(graph_cfg, canvas_format);
    ((rtos_GraphicDisplayDevice *)this->display_device)->check_rtos_display_device_compatibility(graph_cfg, canvas_format);
}

rtos_GraphicWidget::~rtos_GraphicWidget()
{
    delete drawer;
}

rtos_TextWidget::rtos_TextWidget(rtos_Model *actual_displayed_model,
                                 rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{ // TODO add textWriter
}
rtos_TextWidget::~rtos_TextWidget()
{
}

void GraphicDrawer::clear_widget()
{
    canvas->clear_canvas_buffer();
}

GraphicDrawer::GraphicDrawer(struct_ConfigGraphicWidget graph_cfg, CanvasFormat canvas_format)
{
    switch (canvas_format)
    {
    case CanvasFormat::MONO_VLSB:
        this->canvas = new CanvasVLSB(graph_cfg.canvas_width_pixel, graph_cfg.canvas_height_pixel);
        break;
    case CanvasFormat::RGB_COLOR_INDEX_8b:
        this->canvas = new CanvasRGB(graph_cfg.canvas_width_pixel, graph_cfg.canvas_height_pixel);
        break;
    case CanvasFormat::RGB565_16b:
        this->canvas = new CanvasTrueRGB(graph_cfg.canvas_width_pixel, graph_cfg.canvas_height_pixel);
        break;
    default:
        break;
    }
    canvas->fg_color = graph_cfg.canvas_foreground_color;
    canvas->bg_color = graph_cfg.canvas_background_color;

    this->widget_with_border = graph_cfg.widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = canvas->canvas_width_pixel - 2 * widget_border_width;
    widget_height = canvas->canvas_height_pixel - 2 * widget_border_width;
}

GraphicDrawer::~GraphicDrawer()
{
}

void GraphicDrawer::hline(uint8_t x, uint8_t y, size_t w, ColorIndex color)
{
    for (size_t i = 0; i < w; i++)
        canvas->draw_pixel(x + i, y, color);
}

void GraphicDrawer::vline(uint8_t x, uint8_t y, size_t h, ColorIndex color)
{
    for (size_t i = 0; i < h; i++)
        canvas->draw_pixel(x, y + i, color);
}

void GraphicDrawer::line(int x0, int y0, int x1, int y1, ColorIndex color)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true)
    {
        canvas->draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void GraphicDrawer::rect(uint8_t start_x, uint8_t start_y, size_t w, size_t h, bool fill, ColorIndex color)
{
    if (!fill)
    {
        hline(start_x, start_y, w, color);
        hline(start_x, start_y + h - 1, w, color);
        vline(start_x, start_y, h, color);
        vline(start_x + w - 1, start_y, h, color);
    }
    else
        for (size_t i_x = 0; i_x < w; i_x++)
            for (size_t i_y = 0; i_y < h; i_y++)
                canvas->draw_pixel(start_x + i_x, start_y + i_y, color);
}

void GraphicDrawer::circle(int radius, int x_center, int y_center, bool fill, ColorIndex color)
{
    int x, y, m;
    x = 0;
    y = radius;
    m = 5 - 4 * radius;
    while (x <= y)
    {
        if (!fill)
        {
            canvas->draw_pixel(x_center + x, y_center + y, color);
            canvas->draw_pixel(x_center + y, y_center + x, color);
            canvas->draw_pixel(x_center - x, y_center + y, color);
            canvas->draw_pixel(x_center - y, y_center + x, color);
            canvas->draw_pixel(x_center + x, y_center - y, color);
            canvas->draw_pixel(x_center + y, y_center - x, color);
            canvas->draw_pixel(x_center - x, y_center - y, color);
            canvas->draw_pixel(x_center - y, y_center - x, color);
        }
        else
        {
            hline(x_center - x, y_center + y, 2 * x + 2, color);
            hline(x_center - y, y_center + x, 2 * y + 2, color);
            hline(x_center - y, y_center - x, 2 * y + 2, color);
            hline(x_center - x, y_center - y, 2 * x + 2, color);
        }
        if (m > 0)
        {
            y -= 1;
            m -= 8 * y;
        }
        x += 1;
        m += 8 * x + 4;
    }
}
