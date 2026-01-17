#pragma once
#include "sw/ui_core/rtos_ui_core.h"
#include <set>

/// @brief The status of a blinking widget
enum class BlinkingWidgetStatus
{
    /// @brief The widget is in OFF state : Not displayed
    IS_OFF,
    /// @brief The widget is in ON state : Continuously displayed
    IS_ON,
    /// @brief The widget is blinking : alternates between ON and OFF states, or inverting foregound and background colors
    IS_BLINKING,
};

class rtos_BlinkingWidget;
class rtos_Blinker
{
private:
    /// @brief The period of the blinking, in milliseconds
    std::set<rtos_BlinkingWidget *> blinking_widgets;

public:
    uint32_t blink_period_ms;
    rtos_Blinker(uint32_t blink_period_ms);
    ~rtos_Blinker();
    void add_blinking_model(rtos_BlinkingWidget *model);
    void remove_blinking_model(rtos_BlinkingWidget *model);
    void refresh_blinking();
};

class rtos_BlinkingWidget
{
private:
    rtos_Blinker *blinker;
    BlinkingWidgetStatus blinking_status{BlinkingWidgetStatus::IS_OFF};

protected:
    bool current_blink_phase{false};

public:
    rtos_BlinkingWidget();
    ~rtos_BlinkingWidget();
    void start_blinking();
    void stop_blinking();
    void setup_blinker(rtos_Blinker *blinker);
    virtual void blink() = 0;
};

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

class myClockController : public rtos_UIModelManager
{
private:
public:
    myClockController(bool is_wrapable = false);
    ~myClockController();
    void process_control_event(struct_ControlEventData control_event);
};