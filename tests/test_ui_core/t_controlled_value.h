#if !defined(CONTROLLED_VALUE_H)
#define CONTROLLED_VALUE_H

#include "ui_core.h"

/**
 * @brief Construct an implementation of UIControlledIncrementalValue for test_ui_core program.
 * 
 */
class test_IncrementalValue : public UIControlledIncrementalValue
{
private:
    std::string name;

public:
    /**
     * @brief Construct a new Test Incremental Value object
     * 
     * @param _name 
     * @param _min_value 
     * @param _max_value 
     * @param _is_wrappable 
     * @param increment 
     */
    test_IncrementalValue(std::string _name, int _min_value = 0, int _max_value = 10, bool _is_wrappable = false, int increment = 1);
    /**
     * @brief Destroy the Test Incremental Value object
     * 
     */
    ~test_IncrementalValue();
    void process_control_event(ControlEvent _event);
    /**
     * @brief Get the name object
     * 
     * @return std::string 
     */
    std::string get_name();
};

#endif // CONTROLLED_VALUE_H
