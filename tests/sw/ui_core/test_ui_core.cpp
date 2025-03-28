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
#include <map>
#include <string>
#include "t_controlled_value.cpp"
#include "t_manager.cpp"
#include "t_widget_on_serial_monitor.cpp"

#include "device/KY040/ky040.h"


#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21

/// @brief config of the rotary encoder central switch
struct_ConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

/// @brief config of the rotary encoder clock signal
struct_ConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000};

void shared_irq_call_back(uint gpio, uint32_t event_mask);

/// @brief create encoder

KY040 ky040 = KY040(CENTRAL_SWITCH_GPIO,
                    ENCODER_CLK_GPIO,
                    ENCODER_DT_GPIO,
                    shared_irq_call_back,
                    cfg_central_switch,
                    cfg_encoder_clk);

/**
 * @brief define the ISR associated with the clock signal of the rotary encoder
 *
 * @param gpio  The gpio number connected to the clock signal
 * @param event_mask  the IRQ mask, default to GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE
 */
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
void manager_process_control_event(UIControlEvent event);

/**
 * @brief main test and example program about ui_core, with serial terminal as widget substitute.
 *
 * @return int
 */

/// 4- create a manager connected to the rotary encoder.
MyManager manager = MyManager(&ky040);

int main()
{
    /// main steps

    /// 1- setup serial terminal that uses printf().
    stdio_init_all();

    PrinterDevice my_serial_monitor = PrinterDevice(100, 1);

    /// 2- create 3 incremental value object
    MyIncrementalValueModel value_0 = MyIncrementalValueModel("val0", 0, 5, true, 1);
    MyIncrementalValueModel value_1 = MyIncrementalValueModel("val1", 0, 10, false, 1);
    MyIncrementalValueModel value_2 = MyIncrementalValueModel("val2", -20, 3, false, 1);

    /// 3- create 3 serial terminal widget associated with incremental value objects.
    MyIncrementalValueWidgetOnSerialMonitor value_0_widget = MyIncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_0);
    MyIncrementalValueWidgetOnSerialMonitor value_1_widget = MyIncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_1);
    MyIncrementalValueWidgetOnSerialMonitor value_2_widget = MyIncrementalValueWidgetOnSerialMonitor(&my_serial_monitor, &value_2);

    ky040.update_UI_control_event_processor(manager_process_control_event);

    /// 5- create a widget for the manager
    MyManagerWidget manager_widget = MyManagerWidget(&my_serial_monitor, &manager);

    /// 6- create a set of widget and populate it with all above widgets

    std::vector<Widget *> all_widgets;
    all_widgets.push_back(&manager_widget);
    all_widgets.push_back(&value_0_widget);
    all_widgets.push_back(&value_1_widget);
    all_widgets.push_back(&value_2_widget);

    /// 7- set focus on the first incremental value object
    value_0.update_status(ControlledObjectStatus::HAS_FOCUS);
    /// 8- populate manager with managed object
    manager.add_managed_model(&value_0);
    manager.add_managed_model(&value_1);
    manager.add_managed_model(&value_2);

    /// 9- start infinite loop
    while (true)
    {

        /// - sample the rotary encoder central switch
        UIControlEvent event = ky040.process_central_switch_event();

        /// - give the sampled event to the manager, to let it process the event
        manager.process_control_event(event);

        /// - let the set_of_widget execute refresh
        for (auto &&widget : all_widgets)
            widget->draw();

        /// - sleep for 20ms
        sleep_ms(20);
    }
    return 0;
}
void manager_process_control_event(UIControlEvent event)
{
    manager.process_control_event(event);
};
