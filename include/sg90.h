/**
 * @file sg90.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#if !defined(SG90_H)
#define SG90_H

#include "pico/stdlib.h"
#include "hw_pwm.h"


/// @brief arbitrary value, gives resolution step_ns/(pos_max_us - pos_min_us) = 100ppm.
#define STEP_ns 2000 
/// @brief if PHASE_CORRECt = true, channel B is used as a synch pulse in the begining of channel A pulse else channel B is used as a synch pulse in the middle of channel A pulse.
#define PHASE_CORRECT false 
/// @brief 20ms according to SG90 data sheet
#define PERIOD_us 20000 
/// @brief according to data sheet, should be 1000us but is 500us actually
#define T_MIN_us 500 
/// @brief according to data sheet, should be 2000us but is 2500us actually.
#define T_MAX_us 2500

/**
 * @brief 
 * 
 */
struct struct_ConfigSG90
{
    /// @brief brief description
    uint command_pin;
    /// @brief brief description
    uint sync_pin;
    /// @brief brief description
    int pos_min_degree = -90;
    /// @brief brief description
    int pos_max_degree = +90;
};

/**
 * @brief 
 * 
 */
class SG90
{
private:
    PWM *pwm;
    uint command;
    uint sync;
    int pos_min_deg;
    int pos_max_deg;
    float coef_us_per_degree;

public:
    /**
     * @brief Construct a new SG90 object
     * 
     * @param sg90_config 
     */
    SG90(struct_ConfigSG90 sg90_config);
    /**
     * @brief Set the pos object
     * 
     * @param pos 
     */
    void set_pos(int pos);
};

#endif // SG90_H
