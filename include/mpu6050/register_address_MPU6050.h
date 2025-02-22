#pragma once

//---define MPU6050 register addresses---
//---------------------------------------
#define SMPLRT_DIV_RA         0x19 // Register 25 – Sample Rate Divider 
#define CONFIG_RA             0x1A // Register 26 – Configuration  FSYNC and DLPF
#define GYRO_CONFIG_RA        0x1B // Register 27 – Gyroscope Configuration
#define ACCEL_CONFIG_RA       0x1C // Register 28 – Accelerometer Configuration
#define FIFO_EN_RA            0x23 // Register 35 – FIFO Enable
#define I2C_MST_CTRL_RA       0x24 // Register 36 – I2C Master Control 
#define I2C_SLV0_ADDR_RA      0x25 // Registers 37 to 39 – I2C Slave 0 Control
#define I2C_SLV0_REG_RA       0x26
#define I2C_SLV0_CTRL_RA      0x27
#define I2C_SLV1_ADDR_RA      0x28 // Registers 40 to 42 – I2C Slave 1 Control
#define I2C_SLV1_REG_RA       0x29
#define I2C_SLV1_CTRL_RA      0x2A
#define I2C_SLV2_ADDR_RA      0x2B // Registers 43 to 45 – I2C Slave 2 Control
#define I2C_SLV2_REG_RA       0x2C
#define I2C_SLV2_CTRL_RA      0x2D
#define I2C_SLV3_ADDR_RA      0x2E // Registers 46 to 48 – I2C Slave 3 Control 
#define I2C_SLV3_REG_RA       0x2F
#define I2C_SLV3_CTRL_RA      0x30
#define I2C_SLV4_ADDR_RA      0x31 // Registers 49 to 53 – I2C Slave 4 Control  
#define I2C_SLV4_REG_RA       0x32
#define I2C_SLV4_DO_RA        0x33
#define I2C_SLV4_CTRL_RA      0x34
#define I2C_SLV4_DI_RA        0x35
#define I2C_MST_STATUS_RA     0x36 // Register 54 – I2C Master Status
#define INT_PIN_CFG_RA        0x37 // Register 55 – INT Pin / Bypass Enable Configuration
#define INT_ENABLE_RA         0x38 // Register 56 – Interrupt Enable
#define INT_STATUS_RA         0x3A // Register 58 – Interrupt Status
#define ACCEL_XOUT_H_RA       0x3B // Registers 59 to 64 – Accelerometer Measurements
#define ACCEL_YOUT_H_RA       0x3D
#define ACCEL_ZOUT_H_RA       0x3F
#define TEMP_OUT_H_RA         0x41 // Registers 65 and 66 – Temperature Measurement
#define GYRO_XOUT_H_RA        0x43 // Registers 67 to 72 – Gyroscope Measurements
#define GYRO_YOUT_H_RA        0x45
#define GYRO_ZOUT_H_RA        0x47
#define EXT_SENS_DATA_00_RA   0x49 // Registers 73 to 96 – External Sensor Data 
#define I2C_SLV0_DO_RA        0x63 // Register 99 – I2C Slave 0 Data Out
#define I2C_SLV1_DO_RA        0x64 // Register 100 – I2C Slave 1 Data Out
#define I2C_SLV2_DO_RA        0x65 // Register 101 – I2C Slave 2 Data Out
#define I2C_SLV3_DO_RA        0x66 // Register 102 – I2C Slave 3 Data Out
#define I2C_MST_DELAY_CTRL_RA 0x67 // Register 103 – I2C Master Delay Control 
#define SIGNAL_PATH_RESET_RA  0x68 // Register 104 – Signal Path Reset
#define USER_CTRL_RA          0x6A // Register 106 – User Control
#define PWR_MGMT_1_RA         0x6B // Register 107 – Power Management 1
#define PWR_MGMT_2_RA         0x6C // Register 108 – Power Management 2
#define FIFO_COUNT_H_RA       0x72 // Register 114 and 115 – FIFO Count Registers
#define FIFO_COUNT_L_RA       0x73
#define FIFO_R_W_RA           0x74 // Register 116 – FIFO Read Write
#define WHO_AM_I_RA           0x75 // Register 117 – Who Am I


