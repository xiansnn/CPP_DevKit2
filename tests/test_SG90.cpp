/**
 * @file test_SG90.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "pico/stdlib.h"
#include "device_SG90/sg90.h"

struct_ConfigSG90 motor_config{
    .command_pin = 7,
    .sync_pin = 6
};


int main()
{
    SG90 motor = SG90(motor_config);
    int pos_step = 45; // in °
    int pos = motor_config.pos_min_degree;
    bool going_up = true;

    while (true)
    {

        if (going_up)
        {
            pos += pos_step;
            if (pos >= motor_config.pos_max_degree)
            {
                pos = motor_config.pos_max_degree;
                going_up = false;
            }
        }
        else
        {
            pos -= pos_step;
            if (pos <= motor_config.pos_min_degree)
            {
                pos = motor_config.pos_min_degree;
                going_up = true;
            }
        }

        motor.set_pos(pos);
        sleep_ms(1000);
    }

    return 0;
}
