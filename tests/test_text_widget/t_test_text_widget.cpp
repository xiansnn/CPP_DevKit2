/**
 * @file t_test_text_widget.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "ui_core.h"

class MyFullScreenText : public Model
{
private:
    /* data */
public:
    MyFullScreenText(/* args */);
    ~MyFullScreenText();
    void process_control_event(UIControlEvent _event);
};

MyFullScreenText::MyFullScreenText(/* args */) : Model()
{
}

MyFullScreenText::~MyFullScreenText()
{
}

void MyFullScreenText::process_control_event(UIControlEvent _event)
{
    this->set_change_flag();
}
