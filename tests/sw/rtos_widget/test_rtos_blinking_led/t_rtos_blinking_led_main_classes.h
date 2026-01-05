#pragma once
#include "sw/ui_core/rtos_ui_core.h"

class myMainClock;
class myControlledClockTime : public rtos_UIControlledModel, public core_CircularIncremetalControlledModel
{
private:
    /* data */
public:
    myMainClock *parent_model;
    std::string name;
    myControlledClockTime(std::string name, myMainClock *parent_model,
                      int min_value = 0, int max_value = 60, int increment = 1);
    ~myControlledClockTime();
    void process_control_event(struct_ControlEventData control_event);
};

class myMainClock : public rtos_UIControlledModel
{
private:
public:
    myMainClock(/* args */);
    ~myMainClock();
    myControlledClockTime hour;
    myControlledClockTime minute;
    myControlledClockTime second; 
    
    void process_control_event(struct_ControlEventData control_event);
};

class myFocusManager : public rtos_UIModelManager
{
private:
public:
    myFocusManager(bool is_wrapable = false);
    ~myFocusManager();
    void process_control_event(struct_ControlEventData control_event);
};