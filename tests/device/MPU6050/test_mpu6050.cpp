#include "pico/stdlib.h"
#include "pico/stdio.h"
#include <stdio.h>
#include <math.h>

#include "device/mpu6050.h"
#include "utilities/probe.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_STANDARD_MODE};

struct_ConfigMPU6050 mpu_cfg{
    .SAMPLE_RATE = 50,
    .DLPF_BW = 5};

void print_measures(struct_MPUData measures)
{
    printf("AccX = %+.2f\tY = %+.2f\tZ = %+.2f", measures.g_x, measures.g_y, measures.g_z);
    printf("\tvecteur G: %+.2f", sqrt(pow(measures.g_x, 2) + pow(measures.g_y, 2) + pow(measures.g_z, 2)));
    printf("\tGyroX = %+.2f\tY = %+.2f\tZ = %+.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
    printf("\n\n");
}

void print_raw_data(struct_RawData raw_data)
{
    printf("Acc [X = %4x\t\tY = %4x\t\tZ = %4x ]", raw_data.g_x, raw_data.g_y, raw_data.g_z);
    printf("\t\t\tGyro [X = %4x\t\tY = %4x\t\tZ = %4x ]", raw_data.gyro_x, raw_data.gyro_y, raw_data.gyro_z);
    printf("\n\n");
};

int main()
{
    stdio_init_all();
    HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
    MPU6050 mpu = MPU6050(&master, mpu_cfg);
    int32_t sample_period_ms = 1000 / mpu_cfg.SAMPLE_RATE;
    float t = mpu.get_MPU_temperature();
    printf("temperature : %.2f\n", t);
    while (true)
    {
        pr_D5.hi();
        if (mpu.is_data_ready())
        {
            pr_D4.hi();
            struct_I2CXferResult result = mpu.get_measures();
            if (result.error)
                printf("i2c error : %s \n", result.context.c_str());
            else
                // print_raw_data(mpu.raw);
                print_measures(mpu.data);
            pr_D4.lo();
        }
        pr_D5.lo();
        sleep_ms(sample_period_ms);
    }
    return 0;
}
