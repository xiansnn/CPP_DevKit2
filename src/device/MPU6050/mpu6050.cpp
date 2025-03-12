/**
 * @file mpu6050.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "include/register_address_MPU6050.h"
#include "include/mpu6050.h"
#include <stdio.h>
#include <math.h>

MPU6050::MPU6050(HW_I2C_Master *master, struct_ConfigMPU6050 mpu_config)
{
    this->master = master;
    this->device_config = mpu_config;
    this->init_mpu();
    sleep_ms(1);
    this->calibrate();
}

struct_I2CXferResult MPU6050::read_registers_all_raw_data()
{
    struct_I2CXferResult result;
    uint8_t read_buf[14];
    result = this->master->burst_byte_read(this->device_config.MPU_ADDR, ACCEL_XOUT_H_RA, read_buf, 14);
    this->raw.g_x = (read_buf[0] << 8) + read_buf[1];
    this->raw.g_y = (read_buf[2] << 8) + read_buf[3];
    this->raw.g_z = (read_buf[4] << 8) + read_buf[5];
    this->raw.temp_out = (read_buf[6] << 8) + read_buf[7];
    this->raw.gyro_x = (read_buf[8] << 8) + read_buf[9];
    this->raw.gyro_y = (read_buf[10] << 8) + read_buf[11];
    this->raw.gyro_z = (read_buf[12] << 8) + read_buf[13];
    return result;
}

void MPU6050::read_FIFO_all_raw_data()
{
    uint8_t read_buf[14];
    this->master->burst_byte_read(this->device_config.MPU_ADDR, FIFO_R_W_RA, read_buf, 14);

    this->raw.g_x = (read_buf[0] << 8) + read_buf[1];
    this->raw.g_y = (read_buf[2] << 8) + read_buf[3];
    this->raw.g_z = (read_buf[4] << 8) + read_buf[5];
    this->raw.temp_out = (read_buf[6] << 8) + read_buf[7];
    this->raw.gyro_x = (read_buf[8] << 8) + read_buf[9];
    this->raw.gyro_y = (read_buf[10] << 8) + read_buf[11];
    this->raw.gyro_z = (read_buf[12] << 8) + read_buf[13];
}
void MPU6050::convert_raw_to_measure()
{
    this->data.g_x = this->raw.g_x * this->acceleration_factor + this->accel_x_offset;
    this->data.g_y = this->raw.g_y * this->acceleration_factor + this->accel_y_offset;
    this->data.g_z = this->raw.g_z * this->acceleration_factor + this->accel_z_offset;
    this->data.gyro_x = this->raw.gyro_x * this->gyro_factor + this->gyro_x_offset;
    this->data.gyro_y = this->raw.gyro_y * this->gyro_factor + this->gyro_y_offset;
    this->data.gyro_z = this->raw.gyro_z * this->gyro_factor + this->gyro_z_offset;
    this->data.temp_out = this->raw.temp_out * this->temperature_gain + this->temperature_offset;
}

void MPU6050::init_mpu()
{
    this->master->single_byte_write(this->device_config.MPU_ADDR, SIGNAL_PATH_RESET_RA, 0x07);

    this->master->single_byte_write(this->device_config.MPU_ADDR, PWR_MGMT_1_RA, this->device_config.PWR_MGMT_1);
    // device_config FSYNC and DLPF device_config
    if (this->device_config.DLPF_BW == 250)
        this->device_config.GYRO_OUT_RATE = 8000;
    else
        this->device_config.GYRO_OUT_RATE = 1000;
    uint8_t DLPF_CFG;
    if (this->device_config.DLPF_BW == 250)
        DLPF_CFG = DLPF_CFG_250HZ;
    else if (this->device_config.DLPF_BW == 200)
        DLPF_CFG = DLPF_CFG_200HZ;
    else if (this->device_config.DLPF_BW == 100)
        DLPF_CFG = DLPF_CFG_100HZ;
    else if (this->device_config.DLPF_BW == 50)
        DLPF_CFG = DLPF_CFG_50HZ;
    else if (this->device_config.DLPF_BW == 20)
        DLPF_CFG = DLPF_CFG_20HZ;
    else if (this->device_config.DLPF_BW == 10)
        DLPF_CFG = DLPF_CFG_10HZ;
    else if (this->device_config.DLPF_BW == 5)
        DLPF_CFG = DLPF_CFG_5HZ;
    this->master->single_byte_write(this->device_config.MPU_ADDR, CONFIG_RA, this->device_config.EXT_SYNC | DLPF_CFG);
    uint8_t sample_div = int((this->device_config.GYRO_OUT_RATE / this->device_config.SAMPLE_RATE) - 1);
    this->master->single_byte_write(this->device_config.MPU_ADDR, SMPLRT_DIV_RA, sample_div);
    // set gyro sensor resolution
    this->gyro_factor = (float)this->device_config.GYRO_FULL_SCALE_RANGE / 32768.;
    uint8_t GYRO_CONFIG = 0;
    if (this->device_config.GYRO_FULL_SCALE_RANGE == 250)
        GYRO_CONFIG = GYRO_RANGE_250DPS;
    else if (this->device_config.GYRO_FULL_SCALE_RANGE == 500)
        GYRO_CONFIG = GYRO_RANGE_500DPS;
    else if (this->device_config.GYRO_FULL_SCALE_RANGE == 1000)
        GYRO_CONFIG = GYRO_RANGE_1000DPS;
    else if (this->device_config.GYRO_FULL_SCALE_RANGE == 2000)
        GYRO_CONFIG = GYRO_RANGE_2000DPS;
    uint8_t ACCEL_CONFIG = 0;
    if (this->device_config.ACCEL_FULL_SCALE_RANGE == 2)
        ACCEL_CONFIG = ACCEL_RANGE_2G;
    else if (this->device_config.ACCEL_FULL_SCALE_RANGE == 4)
        ACCEL_CONFIG = ACCEL_RANGE_4G;
    else if (this->device_config.ACCEL_FULL_SCALE_RANGE == 8)
        ACCEL_CONFIG = ACCEL_RANGE_8G;
    else if (this->device_config.ACCEL_FULL_SCALE_RANGE == 16)
        ACCEL_CONFIG = ACCEL_RANGE_16G;
    this->master->single_byte_write(this->device_config.MPU_ADDR, GYRO_CONFIG_RA, GYRO_CONFIG);
    // set acceleration sensor resolution
    this->acceleration_factor = (float)this->device_config.ACCEL_FULL_SCALE_RANGE / 32768.;
    this->master->single_byte_write(this->device_config.MPU_ADDR, ACCEL_CONFIG_RA, ACCEL_CONFIG);
    // reset FIFO
    this->master->single_byte_write(this->device_config.MPU_ADDR, USER_CTRL_RA, FIFO_RESET);
    // enable FIFO operations
    this->master->single_byte_write(this->device_config.MPU_ADDR, USER_CTRL_RA, this->device_config.FIFO_OP);
    // configure sensors to write in FIFO
    this->master->single_byte_write(this->device_config.MPU_ADDR, FIFO_EN_RA, this->device_config.FIFO_SELECTED_SENSORS);
    // configure INT
    this->master->single_byte_write(this->device_config.MPU_ADDR, INT_PIN_CFG_RA, this->device_config.INT_PIN_CFG);
    // configure INT on Data ready
    this->master->single_byte_write(this->device_config.MPU_ADDR, INT_ENABLE_RA, this->device_config.INT_ENABLE);
}

void MPU6050::read_FIFO_g_accel_raw_data()
{
    uint8_t read_buf[12];
    this->master->burst_byte_read(this->device_config.MPU_ADDR, FIFO_R_W_RA, read_buf, 12);

    this->raw.g_x = (read_buf[0] << 8) + read_buf[1];
    this->raw.g_y = (read_buf[2] << 8) + read_buf[3];
    this->raw.g_z = (read_buf[4] << 8) + read_buf[5];
    this->raw.gyro_x = (read_buf[6] << 8) + read_buf[7];
    this->raw.gyro_y = (read_buf[8] << 8) + read_buf[9];
    this->raw.gyro_z = (read_buf[10] << 8) + read_buf[11];
}
void MPU6050::read_FIFO_accel_raw_data()
{
    uint8_t read_buf[6];
    this->master->burst_byte_read(this->device_config.MPU_ADDR, FIFO_R_W_RA, read_buf, 6);

    this->raw.g_x = (read_buf[0] << 8) + read_buf[1];
    this->raw.g_y = (read_buf[2] << 8) + read_buf[3];
    this->raw.g_z = (read_buf[4] << 8) + read_buf[5];
}

void MPU6050::calibrate()
{
    float accel_x{};
    float accel_y{};
    float accel_z{};
    float gyro_x{};
    float gyro_y{};
    float gyro_z{};
    size_t nb_sample{100};

    size_t i = 0;
    while (i < nb_sample)
    {
        if (this->is_data_ready())
        {
            this->read_registers_all_raw_data();
            accel_x += (float)this->raw.g_x;
            accel_y += (float)this->raw.g_y;
            accel_z += (float)this->raw.g_z;
            gyro_x += (float)this->raw.gyro_x;
            gyro_y += (float)this->raw.gyro_y;
            gyro_z += (float)this->raw.gyro_z;
            i++;
        }
        sleep_ms(1);
    }
    float ax = accel_x / nb_sample;
    float ay = accel_y / nb_sample;
    float az = accel_z / nb_sample;
    float g = 32768.0 / this->device_config.ACCEL_FULL_SCALE_RANGE;
    float gx = gyro_x / nb_sample;
    float gy = gyro_y / nb_sample;
    float gz = gyro_z / nb_sample;

    this->accel_x_offset = (-ax) * this->acceleration_factor;
    this->accel_y_offset = (-ay) * this->acceleration_factor;
    this->accel_z_offset = (g - az) * this->acceleration_factor;
    this->gyro_x_offset = (-gx) * this->gyro_factor;
    this->gyro_y_offset = (-gy) * this->gyro_factor;
    this->gyro_z_offset = (-gz) * this->gyro_factor;
}

float MPU6050::get_MPU_temperature()
{
    uint8_t read_buf[2];
    this->master->burst_byte_read(this->device_config.MPU_ADDR, TEMP_OUT_H_RA, read_buf, 2);
    this->raw.temp_out = (read_buf[0] << 8) + read_buf[1];
    return (float)this->raw.temp_out * this->temperature_gain + this->temperature_offset;
}

struct_I2CXferResult MPU6050::get_measures()
{
    struct_I2CXferResult result;
    result = this->read_registers_all_raw_data();
    this->convert_raw_to_measure();
    return result;
}

uint16_t MPU6050::get_FIFO_count()
{
    uint8_t read_buf[2] = {FIFO_COUNT_H_RA};
    this->master->burst_byte_read(this->device_config.MPU_ADDR, FIFO_COUNT_H_RA, read_buf, 2);
    return (read_buf[0] << 8) + read_buf[1];
}

bool MPU6050::is_data_ready()
{
    uint8_t status[1];
    this->master->single_byte_read(this->device_config.MPU_ADDR, INT_STATUS_RA, status);

    return status[0] & DATA_RDY_INT;
}
