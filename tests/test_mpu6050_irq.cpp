#include "mpu6050.h"
// #include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include "probe.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D1 = Probe(1);

#define MPU_INT 22 // gpio connnected to MPU INT pin

struct_ConfigMasterI2C cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_STANDARD_MODE};

struct_ConfigMPU6050 mpu_cfg{
    .SAMPLE_RATE = 50,
    .DLPF_BW = 5};

// create I2C bus hw peripheral and MPU
HW_I2C_Master master = HW_I2C_Master(cfg_i2c);
MPU6050 mpu = MPU6050(&master, mpu_cfg);

bool data_ready = false;

void gpio_callback(uint gpio, uint32_t events)
{
    pr_D1.hi();
    mpu.get_measures();
    data_ready = true;
    pr_D1.lo();
}

int main()
{
    stdio_init_all();
    printf("temperature : %.2f\n", mpu.get_MPU_temperature());

    gpio_set_irq_enabled_with_callback(MPU_INT, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while (true)
    {
        pr_D4.hi();
        if (data_ready)
        {
            pr_D5.hi();
            mpu.print_measures();
            data_ready = false;
            pr_D5.lo();
        }
        pr_D4.lo();
        sleep_ms(100);
    }
    return 0;
}