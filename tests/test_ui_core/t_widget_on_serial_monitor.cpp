/**
 * @file t_widget_on_serial_monitor.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "t_controlled_value.cpp"
#include "t_manager.cpp"

#include "ui_core.h"
#include "widget.h"

#include <sstream>
#include <string>

/// @brief This is an implementation of a pseudo-widget for test_ui_core program.
/// It write status and value of test_IncrementalValue on the serial monitor
class MyIncrementalValueWidgetOnSerialMonitor : public Widget
{
private:
    float char_position_slope;
    float char_position_offset;
    uint8_t max_line_width = 21;
    int value_to_char_position();

    MyIncrementalValueModel *actual_displayed_object;

public:
    /// @brief Construct a new Test Cursor Widget With Incremental Value object
    /// @param _actual_displayed_object 
    MyIncrementalValueWidgetOnSerialMonitor(MyIncrementalValueModel *_actual_displayed_object);

    ~MyIncrementalValueWidgetOnSerialMonitor();

    /// @brief Implement a draw_refresh function adapted to the current test program with the private function draw()
    void draw_refresh();
};

/**
 * @brief 
 *
 * 
 *
 */

/// @brief This is an implementation of a pseudo-widget for test_ui_core program.
/// It write status and value of MyManager on the serial monitor
class MyManagerWidget : public Widget
{
private:
    MyManager *actual_displayed_object;

public:
    /// @brief Construct a new MyManagerWidget object
    /// @param _manager 
    MyManagerWidget(MyManager *_manager);

    ~MyManagerWidget();
    
    /// @brief Implement a draw_refresh function adapted to the current test program with the function draw()
    void draw_refresh();

};

/// @brief test the composite widget features
class MySetOfWidget : public Widget
{
private:
public:
    MySetOfWidget();
    ~MySetOfWidget();
    void draw_refresh();
};


std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

MyIncrementalValueWidgetOnSerialMonitor::MyIncrementalValueWidgetOnSerialMonitor(MyIncrementalValueModel *_actual_displayed_object)
    : Widget(nullptr, 128, 8, 0, 0, false)
{
    this->actual_displayed_object = _actual_displayed_object;

    char_position_slope = (max_line_width - 1.) / (actual_displayed_object->get_max_value() - actual_displayed_object->get_min_value());
    char_position_offset = 1 - char_position_slope * actual_displayed_object->get_min_value();
}

MyIncrementalValueWidgetOnSerialMonitor::~MyIncrementalValueWidgetOnSerialMonitor()
{
}

void MyIncrementalValueWidgetOnSerialMonitor::draw_refresh()
{
    if (this->actual_displayed_object->has_changed())
    {
        /// draw()
        switch (actual_displayed_object->get_status())
        {
        case ControlledObjectStatus::IS_WAITING:
            printf("[%s] %s with value=%d\n",
                   actual_displayed_object->get_name().c_str(), status_to_string[actual_displayed_object->get_status()].c_str(), actual_displayed_object->get_value());
            break;
        case ControlledObjectStatus::HAS_FOCUS:
            printf("[%s] %s with value=%d\n",
                   actual_displayed_object->get_name().c_str(), status_to_string[actual_displayed_object->get_status()].c_str(), actual_displayed_object->get_value());
            break;
        case ControlledObjectStatus::IS_ACTIVE:
            printf("[%s] %s with value= %d %*c\n",
                   actual_displayed_object->get_name().c_str(), status_to_string[actual_displayed_object->get_status()].c_str(), actual_displayed_object->get_value(), value_to_char_position(), '|');
            break;
        default:
            break;
        }
        /// end draw()
        this->actual_displayed_object->clear_change_flag();
    }
}

int MyIncrementalValueWidgetOnSerialMonitor::value_to_char_position()
{
    return (char_position_slope * actual_displayed_object->get_value() + char_position_offset);
}

MyManagerWidget::MyManagerWidget(MyManager *_manager)
    : Widget(nullptr, 128, 8, 0, 0, false)
{
    this->actual_displayed_object = _manager;
}

MyManagerWidget::~MyManagerWidget()
{
}

void MyManagerWidget::draw_refresh()
{
    if (this->actual_displayed_object->has_changed())
    { /// draw()
        std::string text = "manager " + status_to_string[actual_displayed_object->get_status()] + " with value=" + std::to_string(actual_displayed_object->get_value()) + "\n";
        printf(text.c_str());
        /// end draw()
        this->actual_displayed_object->clear_change_flag();
    }
}

void MySetOfWidget::draw_refresh()
{
    /// for this specific test, the object test_SetOfWidget has only one purpose : to collect a set of widgets.
    if (widgets.size() != 0)
    {
        for (auto &&w : widgets)
            w->draw_refresh();
    }
}

MySetOfWidget::MySetOfWidget()
    : Widget(nullptr, 128, 8, 0, 0, false)
{
}

MySetOfWidget::~MySetOfWidget()
{
}
