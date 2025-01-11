/**
 * @file sg90.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
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
 * @brief the data set that configures the SG90 motor
 */
struct struct_ConfigSG90
{
    /// @brief the gpio pin used to command the servo motor. Correspond to the B channel of the PWM slice
    uint command_pin;
    /// @brief an auxilliary pulse that can be used as a synchro signal. Correspond to the A channel of the PWM slice
    uint sync_pin; 
    /// @brief minimum position in degrees. Typically 0° or -90°.
    int pos_min_degree = -90;
    /// @brief maximum position in degrees. Typically 180° or +90°.
    int pos_max_degree = +90;
};

/**
 * @brief the class that manages the SG90 step motor
 * \todo //TODO check if step motor is still working
 */
class SG90
{
private:
    /// @brief the PWM slice that handle the SG90 step motor device
    PWM *pwm;
    /// @brief the gpio pin used to command the servo motor. Correspond to the B channel of the PWM slice
    uint command;
    /// @brief n auxilliary pulse that can be used as a synchro signal. Correspond to the A channel of the PWM slice
    uint sync;
    /// @brief minimum position in degrees. Typically 0° or -90°.
    int pos_min_deg;
    /// @brief maximum position in degrees. Typically 180° or +90°.
    int pos_max_deg;
    /// @brief the coversion coefficient in microseconds per degree
    float coef_us_per_degree;

public:
    /**
     * @brief Construct a new SG90 object
     * 
     * @param sg90_config 
     */
    SG90(struct_ConfigSG90 sg90_config);
    /**
     * @brief the command to set the position of the motor.
     * 
     * @param pos position in degrees, between pos_min_deg and pos_max_deg.
     */
    void set_pos(int pos);
};

#endif // SG90_H
