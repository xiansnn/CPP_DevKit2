/**
 * @file mpu6050.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include "pico/stdlib.h"
#include "hw/i2c/hw_i2c.h"
#include "config_MPU6050.h"

/**
 * @brief minimal set of MPU configuration data
 * 
 */
struct struct_ConfigMPU6050
{
    /// @brief MPU address, assuming AD0 pin is low otherwise = 0x69
    uint8_t MPU_ADDR = 0x68;
    /// @brief Reg 0x19: set sensors sample rate in Hz
    uint16_t SAMPLE_RATE = 100;
    /// @brief Reg 0x1A: Digital Low-Pass Filter bandwidth.  values in { 250, 200, 100, 50, 20, 10, 5}Hz
    uint8_t DLPF_BW = 50;
    /// @brief Reg 0x1A: external synchronisation via FSYNC pin.
    uint8_t EXT_SYNC = EXT_SYNC_SET_DISABLED;
    /// @brief Reg 0x1A: in Hz, depending on DLPF value
    uint16_t GYRO_OUT_RATE = 1000;
    /// @brief Reg 0x1B: values in { 250, 500, 1000, 2000} DegreePerSecond
    uint16_t GYRO_FULL_SCALE_RANGE = 250;
    /// @brief Reg 0x1C:values in { 2, 4, 8, 16} G
    uint8_t ACCEL_FULL_SCALE_RANGE = 2;
    /// @brief Reg 0x23:
    uint8_t FIFO_SELECTED_SENSORS = GYRO_FIFO_EN | ACCEL_FIFO_EN;
    /// @brief Reg 0x37: Active LO, open drain, pulsed 50us, cleared any read operation
    uint8_t INT_PIN_CFG = INT_LEVEL | LATCH_INT_EN | INT_RD_CLEAR;
    /// @brief Reg 0x38: INT each time a sensor register write occurs
    uint8_t INT_ENABLE = DATA_RDY_EN;
    /// @brief Reg 0x68: default no reset
    uint8_t SIGNAL_PATH_RESET = NO_PATH_RESET;
    /// @brief Reg 0x6A: FIFO enable
    uint8_t FIFO_OP = FIFO_EN;
    /// @brief Reg 0x6B:
    uint8_t PWR_MGMT_1 = CLKSEL_Z_PLL;
};

/**
 * @brief raw data as they are captured by sensor
 *
 */
struct struct_RawData
{
    /// @brief acceleration (x)
    int16_t g_x;
    /// @brief acceleration (y)
    int16_t g_y;
    /// @brief acceleration (z)
    int16_t g_z;
    /// @brief chip temperature
    int16_t temp_out;
    /// @brief rotation speed(x)
    int16_t gyro_x;
    /// @brief rotation speed(y)
    int16_t gyro_y;
    /// @brief rotation speed(z)
    int16_t gyro_z;
};

/**
 * @brief measured sensor value after scale correction
 *
 */
struct struct_MPUData
{
    /// @brief acceleration (x)
    float g_x;
    /// @brief acceleration (y)
    float g_y;
    /// @brief acceleration (z)
    float g_z;
    /// @brief chip temperature
    float temp_out;
    /// @brief rotation speed(x)
    float gyro_x;
    /// @brief rotation speed(y)
    float gyro_y;
    /// @brief rotation speed(z)
    float gyro_z;
};

/**
 * @brief Class that manage the MPU6050 3-axes accelerometer/gyrometer
 * \ingroup sensor
 * \note I2C signals are very sensitive. May not work properly when wiring is not clean 
 */
class MPU6050
{
private:
    /// @brief the I2C master that control the MPU6050 device
    HW_I2C_Master *master;
    /// @brief the MPU6050 configuration
    struct_ConfigMPU6050 device_config;
    /// @brief the accelerator proportional factor computed according to the device configuration
    float acceleration_factor{};
    /// @brief the gyrometer proportional factor computed according to the device configuration
    float gyro_factor{};
    /// @brief  the proportional factor for the temperature
    float temperature_gain = 1.0 / 340.0;
    /// @brief the temperature offset
    float temperature_offset = 36.53;
    /// @brief the x-acceleration offset
    float accel_x_offset{};
    /// @brief the y-acceleration offset
    float accel_y_offset{};
    /// @brief the z-acceleration offset
    float accel_z_offset{};
    /// @brief the x-gyrometer offset
    float gyro_x_offset{};
    /// @brief the y-gyrometer offset
    float gyro_y_offset{};
    /// @brief the z-gyrometer offset
    float gyro_z_offset{};

    /**
     * @brief initialise the MPU configuration
     *
     */
    void init_mpu();
    /**
     * @brief fill the internal data with raw values from sensors
     *
     * @return struct_I2CXferResult The result of the i2c reading
     */
    struct_I2CXferResult read_registers_all_raw_data();
    /**
     * @brief start the calibration of the MPU and compute gyro and accellero offset and conversion factor
     */
    void calibrate();
    /**
     * @brief fill the internal measures by converting the raw data
     */
    void convert_raw_to_measure();
    /// @brief read raw acceleration and gyrometer data from FIFO
    void read_FIFO_g_accel_raw_data();
    /// @brief read raw acceleration data from FIFO
    void read_FIFO_accel_raw_data();

public:
    /**
     * @brief Construct a new MPU6050 object
     *
     * @param master The associated I2C bus master
     * @param default_config the default config according to struct_ConfigMPU6050
     */
    MPU6050(HW_I2C_Master *master, struct_ConfigMPU6050 default_config);
    /**
     * @brief Get the FIFO count object
     *
     * @return uint16_t
     */
    uint16_t get_FIFO_count();
    /// @brief  the set of measures from sensors
    struct_MPUData data;
    /// @brief the set of raw data from sensors
    struct_RawData raw;
    /**
     * @brief Get the MPU temperature object
     * @return float
     */
    float get_MPU_temperature();
    /**
     * @brief the flag that indicates if sensors data are ready
     *
     * @return true
     * @return false
     */
    bool is_data_ready();
    /**
     * @brief Get the raw data, fill the internal struct_RawData, converts raw data to real measures according to offset and coversion factor.
     *
     * @return struct_MPUData
     */
    struct_I2CXferResult get_measures();
    /// @brief read raw data from FIFO
    /// \bug   //FIXME : check FIFO data reading.... seems not working well
    void read_FIFO_all_raw_data();
};
