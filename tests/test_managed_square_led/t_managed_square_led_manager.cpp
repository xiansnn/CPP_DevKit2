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

#include "sw_ui_core/ui_core.h"
#include "t_managed_square_led_models.cpp"

#define MODEL_OBJECT_STATUS_TIME_OUT_us 3000000

std::map<ControlledObjectStatusTimeOutReason, std::string> reason_to_string{
    {ControlledObjectStatusTimeOutReason::NO_TIME_OUT, "NO_TIME_OUT"},
    {ControlledObjectStatusTimeOutReason::MANAGER_INACTIVE, "MANAGER_INACTIVE"},
    {ControlledObjectStatusTimeOutReason::MANAGED_OBJECT_INACTIVE, "MANAGED_OBJECT_INACTIVE"}};

/**
 * @brief MyManager : Example of final implementation of UIModelManager
 */
class MyManager : public UIModelManager
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
    : UIModelManager()
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
            make_managed_model_active();
#ifdef PRINT_DEBUG
            printf("%s is active \n", ((MyManagedSquareLedModel *)this->current_active_model)->name.c_str());
#endif
        }
        else
        {
            make_manager_active();
#ifdef PRINT_DEBUG
            printf("manager is active, %s has focus \n", ((MyManagedSquareLedModel *)this->managed_models[this->value])->name.c_str());
#endif
        }
        break;
    case UIControlEvent::INCREMENT:
        if (current_active_model == this)
        {
            increment_focus();
            this->set_change_flag();
#ifdef PRINT_DEBUG
            printf("[inc] %s has focus\n", ((MyManagedSquareLedModel *)this->managed_models[this->value])->name.c_str());
#endif
        }
        else
            current_active_model->process_control_event(_event);
        break;
    case UIControlEvent::DECREMENT:
        if (current_active_model == this)
        {
            decrement_focus();
            this->set_change_flag();
#ifdef PRINT_DEBUG
            printf("[dec] %s has focus\n", ((MyManagedSquareLedModel *)this->managed_models[this->value])->name.c_str());
#endif
        }
        else
            current_active_model->process_control_event(_event);
        break;
    case UIControlEvent::TIME_OUT:
#ifdef PRINT_DEBUG
        printf("[manager] central switch button time out\n");
#endif
        break;

    default:
        break;
    }
    ControlledObjectStatusTimeOutReason reason = check_time_out(MODEL_OBJECT_STATUS_TIME_OUT_us);
#ifdef PRINT_DEBUG
    if (reason != ControlledObjectStatusTimeOutReason::NO_TIME_OUT)
        printf("[time out] %s\n", reason_to_string[reason].c_str());
#endif
}
