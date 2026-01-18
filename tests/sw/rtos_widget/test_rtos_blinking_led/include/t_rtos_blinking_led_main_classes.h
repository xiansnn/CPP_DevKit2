#pragma once
#include "sw/ui_core/rtos_ui_core.h"
#include <set>

class rtos_BlinkingWidget;
class rtos_Blinker
{
private:
    /// @brief The period of the blinking, in milliseconds
    std::set<rtos_BlinkingWidget *> blinking_widgets;

public:
    uint32_t blink_period_ms;
    bool current_blink_phase{false};
    rtos_Blinker(uint32_t blink_period_ms);
    ~rtos_Blinker();
    void add_blinking_widget(rtos_BlinkingWidget *widget);
    void remove_blinking_widget(rtos_BlinkingWidget *widget);
    void refresh_blinking();
};

class rtos_BlinkingWidget
{
private:

protected:
    rtos_Blinker *blinker;
    ColorIndex fg_color_backup;
    ColorIndex bg_color_backup;

public:
    rtos_BlinkingWidget();
    ~rtos_BlinkingWidget();
    void start_blinking();
    void stop_blinking();
    void setup_blinking(rtos_Blinker *blinker);
    virtual void save_canvas_color() = 0;
    virtual void restore_canvas_color() = 0;
    virtual void blink() = 0;
    virtual void show_focus() = 0;
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