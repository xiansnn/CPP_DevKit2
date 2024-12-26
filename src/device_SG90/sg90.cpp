/**
 * @file sg90.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "sg90.h"

/**
 * @brief Construct a new SG90::SG90 object.
 *
 * @param command // the gpio pin used to command the servo motor.
 * @param sync // an auxilliary pulse that can be used as a synchro signal.
 * @param pos_min_deg // minimum position in degrees. Typically 0° or -90°.
 * @param pos_max_deg // maximum position in degrees. Typically 180° or +90°.
 */
SG90::SG90(struct_ConfigSG90 cfg)
{
    this->pwm = new PWM(cfg.command_pin, cfg.sync_pin, STEP_ns, PERIOD_us, PHASE_CORRECT);
    this->command = cfg.command_pin;
    this->pos_min_deg = cfg.pos_min_degree;
    this->pos_max_deg = cfg.pos_max_degree;
    this->coef_us_per_degree = (float)(T_MAX_us - T_MIN_us) / (this->pos_max_deg - this->pos_min_deg); //   coef us/degree

    pwm->set_width_nb_of_step(cfg.sync_pin, 1); // set the synch pulse width at 1 step
    pwm->start(true);
}

/**
 * @brief the command to set the position of the motor.
 *
 * @param pos position in degrees, between pos_min_deg and pos_max_deg.
 */
void SG90::set_pos(int pos)
{
    int clamp = ((pos >= this->pos_max_deg) ? this->pos_max_deg : ((pos <= this->pos_min_deg) ? this->pos_min_deg : pos));
    float t = this->coef_us_per_degree * (clamp - this->pos_min_deg) + T_MIN_us;
    float dutycycle = t / PERIOD_us;
    pwm->set_duty_cycle(this->command, dutycycle);
}
