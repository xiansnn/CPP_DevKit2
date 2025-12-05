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
#include "sw/widget/canvas.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <vector>
#include <string>

#include "pico/stdlib.h"

class rtos_Widget;


class rtos_Model;

/// @brief RTOS wrapper for Widget class
/// \ingroup view
class rtos_Widget
{
protected:
    /// @brief a pointer to the Model actually displayed by the widget
    rtos_Model *actual_rtos_displayed_model{nullptr};

    /// @brief A rtos_widget can be composed by several rtos_widgets.
    std::vector<rtos_Widget *> rtos_widgets;

public:
    /// @brief the display device where the attached to the frame buffer
    rtos_DisplayDevice *display_device{nullptr};
    /// @brief location in x of the widget within the hosting framebuffer
    uint8_t widget_anchor_x;
    /// @brief location in y of the widget within the hosting framebuffer
    uint8_t widget_anchor_y;

  
    struct_WidgetDataToGateKeeper widget_data_to_gatekeeper;

    /// @brief FreeRTOS task handle associated to the widget
    TaskHandle_t task_handle;


    /// @brief constructor for RTOS widget
    /// @param actual_displayed_model the displayed model of the widget
    /// @param display_device The display device on which the widget is drawn. This device can be "null".
    rtos_Widget(rtos_Model *actual_displayed_model,
                rtos_DisplayDevice *display_device = nullptr);

    /// @brief destructor for RTOS widget
    ~rtos_Widget();
    /// @brief Set the display screen object
    /// @param _new_display_device
    void set_display_device(rtos_DisplayDevice *_new_display_device);
    /// @brief Modify the anchor of the widget on the display screen
    /// @param x anchor x coordinate
    /// @param y anchor y coordinate
    void update_widget_anchor(uint8_t x, uint8_t y);

    /// @brief add sub_widget to the current widget
    /// @param _sub_widget
    void add_widget(rtos_Widget *_sub_widget);

    /// @brief a pure virtual member that is called to effectively draw the widget.
    /// \note USAGE: This member function can be called by the draw_refresh_all_attached_widgets() method of the Model.
    /// Refer to the following diagram.
    /// \image html draw.svg
    virtual void draw() = 0;
    /// @brief A pure virtual method that results in the transfer of the displayed values of the displayed model to the widget.
    virtual void get_value_of_interest() = 0;
};
class GraphicDrawer;

class rtos_GraphicWidget : public rtos_Widget
{
private:
public:
    GraphicDrawer *drawer;
    rtos_GraphicWidget(rtos_Model *actual_displayed_model,
                       struct_ConfigGraphicWidget graph_cfg,
                       CanvasFormat canvas_format,
                       rtos_DisplayDevice *display_device = nullptr);
    ~rtos_GraphicWidget();

        /// @brief used with FreeRTOS. send the widget data_to_display structure to the task in charge of the display management
    /// @param display_queue the communcation queue with the display gate keeper
    /// @param sending_done the semaphore triggered when the canvas display is complete.
    void send_image_to_DisplayGateKeeper(QueueHandle_t display_queue, SemaphoreHandle_t sending_done);

    /// @brief Send a command to clear the device screen buffer
    /// @param display_queue    the communication queue with the display gate keeper
    /// @param sending_done     the semaphore to signal when the sending is done
    void send_clear_device_command(QueueHandle_t display_queue, SemaphoreHandle_t sending_done);

};

class rtos_TextWidget : public rtos_Widget
{
private:
public:
    rtos_TextWidget(rtos_Model *actual_displayed_model,
                    rtos_DisplayDevice *display_device = nullptr);
    ~rtos_TextWidget();
};

class GraphicDrawer
{
protected:
    /// @brief if true, the widget is surrounded by a one-pixel border
    bool widget_with_border{false};

    /// @brief As a widget can be surrounded by a border, the actual widget width is not the associated framebuffer width.
    size_t widget_width{128};

    /// @brief As a widget can be surrounded by a border, the actual widget height is not the associated framebuffer height.
    size_t widget_height{8};

    /// @brief this is the actual horizontal start of the widget drawing area, taken into account the presence of border.
    /// \note WARNING: when the FramebufferFormat format is MONO_VLSB, works fine only if widget_height is a multiple of 8.
    uint8_t widget_start_x;

    /// @brief this is the actual vertical start of the widget drawing area, taken into account the presence of border.
    /// \note when the FramebufferFormat format is MONO_VLSB, works fine only if widget_start_y is a multiple of 8
    uint8_t widget_start_y;

    /// @brief this is the border size of the widget. 0 if no border, 1 if border
    uint8_t widget_border_width;

    /// @brief fill the graphic pixel buffer with 0x00.
    /// \note USAGE: used at the begining of the draw() method.
    void clear_widget();

public:
    GraphicDrawer(struct_ConfigGraphicWidget graph_cfg,
                  CanvasFormat canvas_format);
    virtual ~GraphicDrawer();

    /// @brief the associated canvas in which the widget writes text and draws graphics
    Canvas *canvas;

    /// @brief Draw a color horizontal line, starting at frame position (x,y), on w number of pixel.
    /// @param x horizontal start of line
    /// @param y vertical start of line
    /// @param w length of the line in number of pixel
    /// @param color color of the line, default to WHITE
    void hline(uint8_t x, uint8_t y, size_t w, ColorIndex color = ColorIndex::WHITE);

    /// @brief Draw a color vertical line, starting at frame position (x,y), on w number of pixel.
    /// @param x horizontal start of line
    /// @param y vertical start of line
    /// @param h length of the line in number of pixel
    /// @param color color of the line, default to WHITE
    void vline(uint8_t x, uint8_t y, size_t h, ColorIndex color = ColorIndex::WHITE);

    /// @brief Draw a color line, starting at frame position (x0,y0), ending at frame position (x1,y1)
    /// @param x0 horizontal start of line
    /// @param y0 vertical start of line
    /// @param x1 horizontal end of line
    /// @param y1 vertical end of line
    /// @param color color of the line, default to WHITE
    void line(int x0, int y0, int x1, int y1, ColorIndex color = ColorIndex::WHITE);

    /// @brief Draw a rectangle, starting at frame position (x,y), w wide and h high
    /// @param start_x horizontal start of the rectangle
    /// @param start_y vertical start of the rectangle
    /// @param w number of pixel of the rectangle width
    /// @param h number of pixel of the rectangle height
    /// @param fill if true, the rectangle is filled with color
    /// @param color color of the border of the rectangle, default to WHITE
    void rect(uint8_t start_x, uint8_t start_y, size_t w, size_t h, bool fill = false, ColorIndex color = ColorIndex::WHITE);
    /**
     * @brief draw a cercle of size radius, centered at (x_center, y_center)
     * https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_d%27arc_de_cercle_de_Bresenham
     * https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
     * procédure tracerCercle (entier rayon, entier x_centre, entier y_centre)
     *     déclarer entier x, y, m ;
     *    x ← 0 ;
     *    y ← rayon ;             // on se place en haut du cercle
     *    m ← 5 - 4*rayon ;       // initialisation
     *    Tant que x <= y         // tant qu'on est dans le second octant
     *        tracerPixel( x+x_centre, y+y_centre ) ;
     *        tracerPixel( y+x_centre, x+y_centre ) ;
     *        tracerPixel( -x+x_centre, y+y_centre ) ;
     *        tracerPixel( -y+x_centre, x+y_centre ) ;
     *        tracerPixel( x+x_centre, -y+y_centre ) ;
     *        tracerPixel( y+x_centre, -x+y_centre ) ;
     *        tracerPixel( -x+x_centre, -y+y_centre ) ;
     *        tracerPixel( -y+x_centre, -x+y_centre ) ;
     *        si m > 0 alors	//choix du point F
     *            y ← y - 1 ;
     *            m ← m - 8*y ;
     *        fin si ;
     *        x ← x + 1 ;
     *        m ← m + 8*x + 4 ;
     *     fin tant que ;
     * fin de procédure ;
     *
     *
     * @param radius   radius, in pixel, of the circle
     * @param x_center   horizontal position of the center of the cercle
     * @param y_center   vertical position of the center on the cercle
     * @param fill   if true, the circle is filled with color c
     * @param color   color of the border of the circle, default to WHITE
     */
    void circle(int radius, int x_center, int y_center, bool fill = false, ColorIndex color = ColorIndex::WHITE);
};
