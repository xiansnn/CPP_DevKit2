/**
 * @file t_managed_square_led_manager.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "ui_core.h"
#include "t_managed_square_led_models.cpp"

#define MODEL_OBJECT_STATUS_TIME_OUT_us 3000000

std::map<ControlledObjectStatusTimeOutReason, std::string> reason_to_string{
    {ControlledObjectStatusTimeOutReason::NO_TIME_OUT, "NO_TIME_OUT"},
    {ControlledObjectStatusTimeOutReason::MANAGER_INACTIVE, "MANAGER_INACTIVE"},
    {ControlledObjectStatusTimeOutReason::FOCUS_LOST, "FOCUS_LOST"}};

/**
 * @brief MyManager : Example of final implementation of UIObjectManager
 */
class MyManager : public UIObjectManager
{
private:
public:
    /**
     * @brief Construct a new MyManager object
     *
     * @param _controller
     */
    MyManager(UIController *_controller);
    /**
     * @brief Destroy the Test_Manager object
     */
    ~MyManager();
    /// @brief function that interprets the event send by the controller.
    /// @param _event
    void process_control_event(UIControlEvent _event);
};

MyManager::MyManager(UIController *_controller)
    : UIObjectManager()
{
    make_manager_active();
    update_current_controller(_controller);
}
MyManager::~MyManager()
{
}
void MyManager::process_control_event(UIControlEvent _event)
{
    switch (_event)
    {
    case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (current_active_model == this)
        {
            make_managed_object_active();
            printf("%s is active \n", ((MySquareLedModel *)this->current_active_model)->name.c_str());
        }
        else
        {
            make_manager_active();
            printf("manager is active, %s has focus \n", ((MySquareLedModel *)this->managed_models[this->value])->name.c_str());
        }
        break;
    case UIControlEvent::INCREMENT:
        if (current_active_model == this)
        {
            increment_focus();
            printf("[inc] %s has focus\n", ((MySquareLedModel *)this->managed_models[this->value])->name.c_str());
        }
        else
            current_active_model->process_control_event(_event);
        break;
    case UIControlEvent::DECREMENT:
        if (current_active_model == this)
        {
            decrement_focus();
            printf("[dec] %s has focus\n", ((MySquareLedModel *)this->managed_models[this->value])->name.c_str());
        }
        else
            current_active_model->process_control_event(_event);
        break;
    case UIControlEvent::TIME_OUT:
        printf("[manager] central switch button time out\n");
        break;

    default:
        break;
    }
    ControlledObjectStatusTimeOutReason reason = check_time_out(MODEL_OBJECT_STATUS_TIME_OUT_us);
    if (reason != ControlledObjectStatusTimeOutReason::NO_TIME_OUT)
        printf("[time out] %s\n", reason_to_string[reason].c_str());
}
