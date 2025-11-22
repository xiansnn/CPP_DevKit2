/**
 * @file rtos_widget.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "sw/display_device/display_device.h"
// #include "sw/ui_core/rtos_ui_core.h"
#include "sw/widget/canvas.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <vector>
#include <string>

#include "pico/stdlib.h"

class rtos_Model;

/// @brief RTOS wrapper for Widget class
/// \ingroup view
class rtos_Widget
{
protected:
    /// @brief a pointer to the Model actually displayed by the widget
    rtos_Model *actual_displayed_model{nullptr};

public:
    /// @brief the display device where the attached to the frame buffer
    DisplayDevice *display_device{nullptr};

    /// @brief FreeRTOS task handle associated to the widget
    TaskHandle_t task_handle;
    /// @brief the data structure used to send the canvas to the display task when a FreeRTOS queue is used.
    struct_DataToShow data_to_display;

    /// @brief used with FreeRTOS. send the widget data_to_display structure to the task in charge of the display management
    /// @param display_queue the communcation queue with the display gate keeper
    /// @param sending_done the semaphore triggered when the canvas display is complete.
    void send_image_to_DisplayGateKeeper(QueueHandle_t display_queue, SemaphoreHandle_t sending_done);

    /// @brief Send a command to clear the device screen buffer
    /// @param display_queue    the communication queue with the display gate keeper
    /// @param sending_done     the semaphore to signal when the sending is done
    void send_clear_device_command(QueueHandle_t display_queue, SemaphoreHandle_t sending_done);
    /// @brief constructor for RTOS widget
    /// @param actual_displayed_model the displayed model of the widget
    /// @param display_device The display device on which the widget is drawn. This device can be "null".

    rtos_Widget(rtos_Model *actual_displayed_model,
                DisplayDevice *display_device = nullptr);

    /// @brief destructor for RTOS widget
    ~rtos_Widget();
    /// @brief Set the display screen object
    /// @param _new_display_device
    void set_display_device(DisplayDevice *_new_display_device);

    /// @brief a pure virtual member that is called to effectively draw the widget.
    /// \note USAGE: This member function can be called by the draw_refresh_all_attached_widgets() method of the Model.
    /// Refer to the following diagram.
    /// \image html draw.svg
    virtual void draw() = 0;
};

class rtos_PrintWidget : public rtos_Widget
{
private:
    /* data */
public:
    rtos_PrintWidget(PrinterDevice *display_device, rtos_Model *actual_displayed_model = nullptr);
    ~rtos_PrintWidget();
};

