#include "rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"
#include <cstring>

rtos_Widget::rtos_Widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device)
{
    this->display_device = display_device;
    this->widget_data_to_gatekeeper.display = this->display_device;
    this->widget_data_to_gatekeeper.widget = this;

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

    drawer = new GraphicDrawer(graph_cfg, canvas_format);
    ((rtos_GraphicDisplayDevice *)this->display_device)->check_rtos_display_device_compatibility(graph_cfg, canvas_format);
}

rtos_GraphicWidget::~rtos_GraphicWidget()
{
    delete drawer;
}

rtos_TextWidget::rtos_TextWidget(rtos_Model *actual_displayed_model,
                                 struct_ConfigTextWidget text_cfg,
                                 CanvasFormat canvas_format,
                                 rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
    writer = new TextWriter(text_cfg, canvas_format);
    this->widget_anchor_x = text_cfg.widget_anchor_x;
    this->widget_anchor_y = text_cfg.widget_anchor_y;
}

rtos_TextWidget::rtos_TextWidget(rtos_Model *actual_displayed_model,
                                 struct_ConfigTextWidget text_cfg,
                                 CanvasFormat canvas_format,
                                 size_t frame_width, size_t frame_height,
                                 rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
    writer = new TextWriter(text_cfg, canvas_format, frame_width, frame_height);
    this->widget_anchor_x = text_cfg.widget_anchor_x;
    this->widget_anchor_y = text_cfg.widget_anchor_y;
}

rtos_TextWidget::~rtos_TextWidget()
{
    delete writer;
}

rtos_PrintWidget::rtos_PrintWidget(rtos_Model *actual_displayed_model,
                                   rtos_TerminalConsole *terminal_console)
    : rtos_Widget(actual_displayed_model, terminal_console)
{
    this->text_buffer = new char[terminal_console->text_buffer_size];
}

rtos_PrintWidget::~rtos_PrintWidget()
{
    delete[] text_buffer;
}
