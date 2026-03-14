#pragma once

#include "sw/ui_core/ui_core.h"
#include "t_managed_square_led_models.h"


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
    void process_control_event(struct_ControlEventData control_event);
};
