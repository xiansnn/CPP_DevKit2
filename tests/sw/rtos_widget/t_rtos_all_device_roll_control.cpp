#include "t_rtos_all_device_roll_control.h"

my_ControlledRollPosition::my_ControlledRollPosition(int _min_value, int _max_value, bool _is_wrappable, int increment)
:core_IncrementControlledModel(_min_value, _max_value, _is_wrappable, increment), rtos_UIControlledModel()
{
}

my_ControlledRollPosition::~my_ControlledRollPosition()
{
}

void my_ControlledRollPosition::process_control_event(struct_ControlEventData control_event)
{
}

my_RollController::my_RollController(bool is_wrapable)
{
}

my_RollController::~my_RollController()
{
}

void my_RollController::process_control_event(struct_ControlEventData control_event)
{
}
