/**
 * @file mpu6050.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#if !defined(MPU6050_H)
#define MPU6050_H

#include "pico/stdlib.h"
#include "hw_i2c.h"
#include "mpu6050/config_MPU6050.h"

/**
 * @brief minimal set of MPU configuration data
 *
 */
struct struct_ConfigMPU6050
{
    uint8_t MPU_ADDR = 0x68;                                       // assuming AD0 pin is low otherwise = 0x69
    uint16_t SAMPLE_RATE = 100;                                    // Reg 0x19: set sensors sample rate in Hz
    uint8_t DLPF_BW = 50;                                          // Reg 0x1A: values in { 250, 200, 100, 50, 20, 10, 5}Hz
    uint8_t EXT_SYNC = EXT_SYNC_SET_DISABLED;                      // Reg 0x1A:
    uint16_t GYRO_OUT_RATE = 1000;                                 // Reg 0x1A: in Hz, depending on DLPF value
    uint16_t GYRO_FULL_SCALE_RANGE = 250;                          // Reg 0x1B:values in { 250, 500, 1000, 2000} DPS
    uint8_t ACCEL_FULL_SCALE_RANGE = 2;                            // Reg 0x1C:values in { 2, 4, 8, 16} G
    uint8_t FIFO_SELECTED_SENSORS = GYRO_FIFO_EN | ACCEL_FIFO_EN;  // Reg 0x23:
    uint8_t INT_PIN_CFG = INT_LEVEL | LATCH_INT_EN | INT_RD_CLEAR; // Reg 0x37: Active LO, open drain, pulsed 50us, cleared any read operation
    uint8_t INT_ENABLE = DATA_RDY_EN;                              // Reg 0x38: INT each time a sensor register write occurs
    uint8_t SIGNAL_PATH_RESET = NO_PATH_RESET;                     // Reg 0x68: default no reset
    uint8_t FIFO_OP = FIFO_EN;                                     // Reg 0x6A: FIFO enable
    uint8_t PWR_MGMT_1 = CLKSEL_Z_PLL;                             // Reg 0x6B:
};

/**
 * @brief raw data as they are captured by sensor
 *
 */
struct struct_RawData
{
    int16_t g_x;
    int16_t g_y;
    int16_t g_z;
    int16_t temp_out;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
};

/**
 * @brief measured sensor value after scale correction
 *
 */
struct struct_MPUData
{
    float g_x;
    float g_y;
    float g_z;
    float temp_out;
    float gyro_x;
    float gyro_y;
    float gyro_z;
};

class MPU6050
{
private:
    HW_I2C_Master *master;
    struct_ConfigMPU6050 device_config;
    float acceleration_factor{};
    float gyro_factor{};
    float temperature_gain = 1.0 / 340.0;
    float temperature_offset = 36.53;
    float accel_x_offset{};
    float accel_y_offset{};
    float accel_z_offset{};
    float gyro_x_offset{};
    float gyro_y_offset{};
    float gyro_z_offset{};
    struct_MPUData data;
    struct_RawData raw;

    void init_mpu();
    void read_registers_all_raw_data();
    void calibrate();
    void convert_raw_to_measure();
    void read_FIFO_all_raw_data();     // FIXME check FIFO data reading.... seems not working well
    void read_FIFO_g_accel_raw_data(); // FIXME check FIFO data reading.... seems not working well
    void read_FIFO_accel_raw_data();   // FIXME check FIFO data reading.... seems not working well

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
    /**
     * @brief Get the MPU temperature object
     *
     * @return float
     */
    float get_MPU_temperature();
    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool is_data_ready();
    /**
     * @brief Get the measures object
     *
     * @return struct_MPUData
     */
    struct_MPUData get_measures();

    struct_RawData get_raw_data();
};

#endif // MPU6050_H