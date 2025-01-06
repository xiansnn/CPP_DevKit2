#if !defined(TEST_MANAGER_H)
#define TEST_MANAGER_H

#include "ui_core.h"

/**
 * @brief implement a UIObjectManager for test_ui_core program
 *
 */
class test_Manager : public UIObjectManager
{
private:
public:
    /**
     * @brief Construct a new test_Manager object
     * 
     * @param _controller 
     */
    test_Manager(UIController *_controller);
    /**
     * @brief Destroy the Test_Manager object
     * 
     */
    ~test_Manager();
    void process_control_event(ControlEvent _event);
};

#endif // TEST_MANAGER_H
