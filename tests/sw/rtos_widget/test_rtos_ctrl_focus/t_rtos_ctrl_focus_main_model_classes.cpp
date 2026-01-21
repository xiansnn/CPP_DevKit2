/**
 * @file t_rtos_extended_roll_control.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-12-27
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "t_rtos_ctrl_focus_config.h"
#include "t_rtos_ctrl_focus_main_model_classes.h"

#include <map>
std::map<UIControlEvent, std::string> event_to_string{
    {UIControlEvent::NONE, "NONE"},
    {UIControlEvent::PUSH, "PUSH"},
    {UIControlEvent::DOUBLE_PUSH, "DOUBLE_PUSH"},
    {UIControlEvent::LONG_PUSH, "LONG_PUSH"},
    {UIControlEvent::RELEASED_AFTER_LONG_TIME, "RELEASED_AFTER_LONG_TIME"},
    {UIControlEvent::RELEASED_AFTER_SHORT_TIME, "RELEASED_AFTER_SHORT_TIME"},
    {UIControlEvent::INCREMENT, "INCREMENT"},
    {UIControlEvent::DECREMENT, "DECREMENT"},
    {UIControlEvent::TIME_OUT, "TIME_OUT"}};

my_model::my_model()
    : rtos_UIControlledModel(),
      angle("ANGLE", this, -180, 180, ANGLE_INCREMENT),  // angle position
      x_pos("H_POS", this, -64, +63, false, 1),
      y_pos("V_POS", this, -28, +27, false, 1)
{
}
my_model::~my_model()
{
}


/// @brief  this is used only with the periodic task
/// @param control_event
void my_model::process_control_event(struct_ControlEventData control_event)
{
    switch (control_event.event)
    {
    case UIControlEvent::INCREMENT:
        angle.increment_value();
        notify_all_linked_widget_task();
        break;

    default:
        break;
    }
}

my_ControlledCenterPosition::my_ControlledCenterPosition(std::string name, my_model *parent_model,
                                                         int min_value, int max_value, bool is_wrappable, int increment)
    : rtos_UIControlledModel(), core_IncrementControlledModel(min_value, max_value, is_wrappable, increment)
{
    this->name = name;
    this->parent_model = parent_model;
}

my_ControlledCenterPosition::~my_ControlledCenterPosition()
{
}

void my_ControlledCenterPosition::process_control_event(struct_ControlEventData control_event)
{
    switch (control_event.event)
    {
    case UIControlEvent::LONG_PUSH:
        this->set_clipped_value(0);
        this->parent_model->notify_all_linked_widget_task();
        break;
    case UIControlEvent::INCREMENT:
        this->increment_value();
        this->parent_model->notify_all_linked_widget_task();
        break;
    case UIControlEvent::DECREMENT:
        this->decrement_value();
        this->parent_model->notify_all_linked_widget_task();
        break;
    default:
        break;
    }
}

my_PositionController::my_PositionController(bool is_wrapable)
    : rtos_UIModelManager(is_wrapable)
{
}

my_PositionController::~my_PositionController()
{
}

void my_PositionController::process_control_event(struct_ControlEventData control_event)
{ // TODO consider an other policy where short release wakeup manager and changes the focus
    if (this->get_rtos_status() == ControlledObjectStatus::IS_IDLE)
    {
        printf("position_controller: WAKE_UP\n");
        this->make_rtos_manager_active();
    }
    else
    {
        if (this->current_active_rtos_model == this)
        {
            switch (control_event.event)
            {
            case UIControlEvent::LONG_PUSH:
                printf("position_controller: LONG_PUSH\n");
                for (auto &&i : managed_rtos_models)
                    ((my_ControlledCenterPosition *)i)->set_clipped_value(0);
                ((my_ControlledCenterPosition *)managed_rtos_models[0])->parent_model->notify_all_linked_widget_task();
                break;
            case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
                printf("position_controller focus on [%s]: RELEASED_AFTER_SHORT_TIME\n", ((my_ControlledCenterPosition *)managed_rtos_models[get_current_focus_index()])->name.c_str());
                this->make_managed_rtos_model_active();
                this->forward_control_event_to_active_managed_model(&control_event);
                break;
            case UIControlEvent::INCREMENT:
                this->increment_focus();
                printf("position_controller focus on [%s]: INCREMENT\n", ((my_ControlledCenterPosition *)managed_rtos_models[get_current_focus_index()])->name.c_str());
                break;
            case UIControlEvent::DECREMENT:
                this->decrement_focus();
                printf("position_controller focus on [%s]: DECREMENT\n", ((my_ControlledCenterPosition *)managed_rtos_models[get_current_focus_index()])->name.c_str());
                break;
            case UIControlEvent::TIME_OUT:
                printf("position_controller focus on [%s]: TIME_OUT\n", ((my_ControlledCenterPosition *)managed_rtos_models[get_current_focus_index()])->name.c_str());
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                break;
            default:
                break;
            }
        }
        else
        {
            switch (control_event.event)
            {
            case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
                printf("make_rtos_manager_active(): RELEASED_AFTER_SHORT_TIME\n");
                this->make_rtos_manager_active();
                break;
            case UIControlEvent::TIME_OUT:
                printf("controlled_position: TIME_OUT\n");
                this->current_active_rtos_model->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                this->current_active_rtos_model = this;
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                break;
            default:
                this->forward_control_event_to_active_managed_model(&control_event);
                break;
            }
        }
    }
}

my_position_controller_widget::my_position_controller_widget(rtos_GraphicDisplayDevice *graphic_display_screen, struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_UIModelManager *manager)
    : rtos_TextWidget(manager, text_cfg, format, graphic_display_screen)
{
}

my_position_controller_widget::~my_position_controller_widget()
{
}

void my_position_controller_widget::get_value_of_interest()
{
    uint idx = ((my_PositionController *)actual_rtos_displayed_model)->get_current_focus_index();
    rtos_UIControlledModel *position = ((my_PositionController *)actual_rtos_displayed_model)->managed_rtos_models[idx];
    focus_on_value_name = ((my_ControlledCenterPosition *)position)->name;
    manager_status = ((my_PositionController *)actual_rtos_displayed_model)->get_rtos_status();
}

void my_position_controller_widget::draw()
{
    this->writer->clear_text_buffer();
    get_value_of_interest();
    // draw
    switch (manager_status)
    {
    case ControlledObjectStatus::HAS_FOCUS:
        printf("(my_position_controller_widget)manager HAS_FOCUS\n");
        break;
    case ControlledObjectStatus::IS_ACTIVE:
        printf("(my_position_controller_widget)manager IS_ACTIVE\n");
        sprintf(this->writer->text_buffer, "%5s", focus_on_value_name.c_str());
        this->writer->write();
        break;
    case ControlledObjectStatus::IS_IDLE:
        printf("(my_position_controller_widget)manager IS_IDLE\n");
        break;
    case ControlledObjectStatus::IS_WAITING:
        printf("(my_position_controller_widget)manager IS_WAITING\n");
        sprintf(this->writer->text_buffer, "%5s", focus_on_value_name.c_str());
        this->writer->write();
        this->writer->draw_border();

        break;
    default:
        break;
    }
}

my_ControlledAnglePosition::my_ControlledAnglePosition(std::string name, my_model *parent_model,
                                                       int min_value, int max_value, int increment)
    : rtos_UIControlledModel(), core_CircularIncremetalControlledModel(increment, min_value, max_value)
{
    this->name = name;
    this->parent_model = parent_model;
}

my_ControlledAnglePosition::~my_ControlledAnglePosition()
{
}

void my_ControlledAnglePosition::process_control_event(struct_ControlEventData control_event)
{
    switch (control_event.event)
    {
    case UIControlEvent::LONG_PUSH:
        this->set_clipped_value(0);
        this->parent_model->notify_all_linked_widget_task();
        break;
    case UIControlEvent::INCREMENT:
        this->increment_value();
        this->parent_model->notify_all_linked_widget_task();
        break;
    case UIControlEvent::DECREMENT:
        this->decrement_value();
        this->parent_model->notify_all_linked_widget_task();
        break;
    default:
        break;
    }
}
