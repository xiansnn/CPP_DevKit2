/**
 * @file test_ui_core.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief This is the main test program of the UI_core.
 *
 * It consists in simulating 3 controlled incremental values, a display widget on the serial monitor and and a rotary encoder as control device,
 * and a View/Control Manager.
 *
 * If the manager IS_ACTIVE, rotating the encoder changes the focus, a short click on the encoder central switch, makes IS_ACTIVE the current controlled value with focus.
 * Rotating the encoder changes now the value of the controlledIncrementalValue. A short click, now, validates the value and pass the control to the manager.
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "test_ui_core/t_controlled_value.h"
#include "test_ui_core/t_manager.h"
#include "test_ui_core/t_widget_on_serial_monitor.h"
#include "ky040.h"
#include <map>
#include <string>
#include "probe.h"

/// @brief  3 probes are create to observe the time execution with a logic analyser
Probe pr_D1 = Probe(1);
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21

/// @brief config of the rotary encoder central switch
struct_SwitchButtonConfig cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

/// @brief config of the rotary encoder clock signal
struct_SwitchButtonConfig cfg_encoder_clk{
    .debounce_delay_us = 5000};

void shared_irq_call_back(uint gpio, uint32_t event_mask);

/// @brief create encoder

KY040 ky040 = KY040(CENTRAL_SWITCH_GPIO,
                    ENCODER_CLK_GPIO,
                    ENCODER_DT_GPIO,
                    shared_irq_call_back,
                    cfg_central_switch,
                    cfg_encoder_clk);


/// @brief define the ISR associated with the clock signal of the rotary encoder
/// @param gpio The gpio number connected to the clock signal
/// @param event_mask the IRQ mask, default to GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE
void shared_irq_call_back(uint gpio, uint32_t event_mask)
{
    pr_D1.pulse_us(10);
    switch (gpio)
    {
    case ENCODER_CLK_GPIO:
        ky040.process_encoder_IRQ(event_mask);
        break;
    default:
        printf("unknown IRQ\n");
        break;
    };
}
void manager_event_processor(UIControlEvent event);

/**
 * @brief test and example program about ui_core, with serial terminal as widget substitute.
 *
 * @return int
 */
/// 4- create a manager connected to the rotary encoder.
test_Manager manager = test_Manager(&ky040);

int main()
{

    /// main steps
    pr_D1.hi();
    /// 1- setup serial terminal that uses printf().
    stdio_init_all();

    /// 2- create 3 incremental value object
    test_IncrementalValue value_0 = test_IncrementalValue("val0", 0, 5, true, 1);
    test_IncrementalValue value_1 = test_IncrementalValue("val1", 0, 10, false, 1);
    test_IncrementalValue value_2 = test_IncrementalValue("val2", -20, 3, false, 1);

    /// 3- create 3 serial terminal widget associated with incremental value objects.
    test_CursorWidgetWithIncrementalValue value_0_widget = test_CursorWidgetWithIncrementalValue(&value_0);
    test_CursorWidgetWithIncrementalValue value_1_widget = test_CursorWidgetWithIncrementalValue(&value_1);
    test_CursorWidgetWithIncrementalValue value_2_widget = test_CursorWidgetWithIncrementalValue(&value_2);

    ky040.update_UI_control_event_processor(manager_event_processor);

    /// 5- create a widget for the manager
    test_ObjectManagerWidget manager_widget = test_ObjectManagerWidget(&manager);

    /// 6- create a set of widget and populate it with all above widgets
    test_SetOfWidget set_of_widget = test_SetOfWidget();
    set_of_widget.add_widget(&manager_widget);
    set_of_widget.add_widget(&value_0_widget);
    set_of_widget.add_widget(&value_1_widget);
    set_of_widget.add_widget(&value_2_widget);

    /// 7- set focus on the first incremental value object
    value_0.update_status(ControlledObjectStatus::HAS_FOCUS);
    /// 8- populate manager with managed object
    manager.add_managed_model(&value_0);
    manager.add_managed_model(&value_1);
    manager.add_managed_model(&value_2);

    pr_D1.lo();

    /// 9- start infinite loop
    while (true)
    {
        pr_D4.pulse_us(10);
        /// - sample the rotary encoder central switch
        UIControlEvent event = ky040.process_central_switch_event();

        /// - give the sampled event to the manager, to let it process the event
        manager.process_control_event(event);
        pr_D5.hi();
        /// - let the set_of_widget execute draw_refresh
        set_of_widget.draw_refresh();
        pr_D5.lo();
        /// - sleep for 20ms
        sleep_ms(20);
    }
    return 0;
}
void manager_event_processor(UIControlEvent event) {
    manager.process_control_event(event);
};
