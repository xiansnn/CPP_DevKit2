#if !defined(WIDGET_PRINT_SERIAL_MONITOR_H)
#define WIDGET_PRINT_SERIAL_MONITOR_H

#include <string>
#include "t_controlled_value.h"
#include "t_manager.h"

#include "ui_core.h"

/**
 * @brief This is an implementation of a pseudo-widget for test_ui_core program.
 *
 * It write status and value of test_IncrementalValue on the serial monitor
 *
 */
class test_CursorWidgetWithIncrementalValue : public UIWidget
{
private:
    float char_position_slope;
    float char_position_offset;
    uint8_t max_line_width = 21;
    int value_to_char_position();

    test_IncrementalValue *actual_displayed_object;

public:
    /**
     * @brief Construct a new Test Cursor Widget With Incremental Value object
     *
     * @param _actual_displayed_object
     */
    test_CursorWidgetWithIncrementalValue(test_IncrementalValue *_actual_displayed_object);
    /**
     * @brief Destroy the Test Cursor Widget With Incremental Value object
     *
     */
    ~test_CursorWidgetWithIncrementalValue();
    /**
     * @brief Implement a draw_refresh function adapted to the current test program with the private function draw()
     *
     */
    void draw_refresh();
};

/**
 * @brief This is an implementation of a pseudo-widget for test_ui_core program.
 *
 * It write status and value of Test_Manager on the serial monitor
 *
 */
class test_ObjectManagerWidget : public UIWidget
{
private:
    test_Manager *actual_displayed_object;

public:
    /**
     * @brief Construct a new Test Object Manager Widget object
     *
     * @param _manager
     */
    test_ObjectManagerWidget(test_Manager *_manager);
    /**
     * @brief Destroy the Test Object Manager Widget object
     *
     */
    ~test_ObjectManagerWidget();
    /**
     * @brief Implement a draw_refresh function adapted to the current test program with the private function draw()
     *
     */
    void draw_refresh();

};

/**
 * @brief test the composite widget features
 *
 */
class test_SetOfWidget : public UIWidget
{
private:
    /* data */
public:
    /**
     * @brief
     *
     */
    test_SetOfWidget(/* args */);
    /**
     * @brief Destroy the Test Set Of Widget object
     *
     */
    ~test_SetOfWidget();
    void draw_refresh();
};

#endif // WIDGET_PRINT_SERIAL_MONITOR_H
