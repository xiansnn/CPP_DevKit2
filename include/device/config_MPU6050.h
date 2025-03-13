#pragma once

//-----------------------------------------------------------------------------
//                               MPU device_config
//-----------------------------------------------------------------------------

//set Digital Low-Pass Filter bandwidth
//                                         Accelero           |                       Gyro
#define DLPF_CFG_250HZ 0x00  // BW = 260Hz | delai =  0.0ms   |   BW = 256Hz | delai =  0.98ms | gyro_sampling_rate = 8kHz
#define DLPF_CFG_200HZ 0x01  // BW = 184Hz | delai =  2.0ms   |   BW = 188Hz | delai =  1.90ms | gyro_sampling_rate = 1kHz
#define DLPF_CFG_100HZ 0x02  // BW =  94Hz | delai =  3.0ms   |   BW =  98Hz | delai =  2.80ms | gyro_sampling_rate = 1kHz 
#define DLPF_CFG_50HZ  0x03  // BW =  44Hz | delai =  4.9ms   |   BW =  42Hz | delai =  4.80ms | gyro_sampling_rate = 1kHz
#define DLPF_CFG_20HZ  0x04  // BW =  21Hz | delai =  8.5ms   |   BW =  20Hz | delai =  8.30ms | gyro_sampling_rate = 1kHz
#define DLPF_CFG_10HZ  0x05  // BW =  10Hz | delai = 13.8ms   |   BW =  10Hz | delai = 13.40ms | gyro_sampling_rate = 1kHz  
#define DLPF_CFG_5HZ   0x06  // BW =   5Hz | delai = 19.0ms   |   BW =   5Hz | delai = 18.60ms | gyro_sampling_rate = 1kHz

// set external FSYNC
// values to be selected
#define  EXT_SYNC_SET_DISABLED     0x00 <<3 
#define  EXT_SYNC_SET_TEMP_OUT_L   0x01 <<3 
#define  EXT_SYNC_SET_GYRO_XOUT_L  0x02 <<3 
#define  EXT_SYNC_SET_GYRO_YOUT_L  0x03 <<3 
#define  EXT_SYNC_SET_GYRO_ZOUT_L  0x04 <<3 
#define  EXT_SYNC_SET_ACCEL_XOUT_L 0x05 <<3 
#define  EXT_SYNC_SET_ACCEL_YOUT_L 0x06 <<3 
#define  EXT_SYNC_SET_ACCEL_ZOUT_L 0x07 <<3
// end values

// ==== Register 27 0x1B– Gyroscope Configuration 
#define GYRO_RANGE_250DPS 0x00 << 3
#define GYRO_RANGE_500DPS 0x01 << 3
#define GYRO_RANGE_1000DPS 0x02 << 3
#define GYRO_RANGE_2000DPS 0x03 << 3


// ==== Register 28 0x1C– Accelerometer Configuration 
#define ACCEL_RANGE_2G 0x00 << 3
#define ACCEL_RANGE_4G 0x01 << 3
#define ACCEL_RANGE_8G 0x02 << 3
#define ACCEL_RANGE_16G 0x03 << 3


// ==== Register 35 0x23– FIFO Enable : device_config which measures will be written into the FIFO
// values to be selected :
#define FIFO_NO_SELECTION 0x00
#define GYRO_FIFO_EN XG_FIFO_EN | YG_FIFO_EN | ZG_FIFO_EN
#define TEMP_FIFO_EN 0x80
#define XG_FIFO_EN 0x40
#define YG_FIFO_EN 0x20
#define ZG_FIFO_EN 0x10
#define ACCEL_FIFO_EN 0x08
#define SLV2_FIFO_EN 0x04
#define SLV1_FIFO_EN 0x02
#define SLV0_FIFO_EN 0x01
// end values

// ==== Register 55 0x37– INT Pin / Bypass Enable Configuration
// values to be selected :
#define INT_LEVEL 0x80    // INT pin active LOW
#define INT_OPEN 0x40     // INT pin open drain
#define LATCH_INT_EN 0x20 // INT pin held Hi until INT cleared
#define INT_RD_CLEAR 0x10 // INT pin cleared by any read
#define FSYNC_INT_LEVEL 0x08
#define FSYNC_INT_EN 0x04
#define I2C_BYPASS_EN 0x02
// end values

// ==== Register 56 0x38– Interrupt Enable
// values to be selected :
#define FIFO_OFLOW_EN 0x10
#define I2C_MST_INT_EN 0x08
#define DATA_RDY_EN 0x01
// end values

// ==== Register 58 0x3A– read Interrupt Status
// values explaining reading
#define FIFO_OFLOW_INT 0x10
#define I2C_MST_INT 0x08
#define DATA_RDY_INT 0x01
// end values

// ==== Register 104 0x68– Signal Path Reset
// values to be selected
#define NO_PATH_RESET 0x00
#define GYRO_RESET 0x04
#define ACCEL_RESET 0x02
#define TEMP_RESET 0x01
// end values

// ==== Register 106 0x6A– User Control
// device_config FIFO, I2C, signal path
#define FIFO_DISABLE 0x00   // disable FIFO operations
#define FIFO_EN 0x40        // enables FIFO operations
#define I2C_MST_EN 0x20     // enables I2C master mode
#define I2C_IF_DIS 0x10     // MPU6000 only
#define FIFO_RESET 0x04     // reset and disables FIFO
#define I2C_MST_RESET 0x02  // reset I2C master while I2C_MST_EN = 0x00
#define SIG_COND_RESET 0x01 // reset all sensors processing and registers

// Register 107 0x6B– Power Management 1
// values to be selected as CLKSEL
#define CLKSEL_INTERNAL 0x00
#define CLKSEL_X_PLL 0x01
#define CLKSEL_Y_PLL 0x02
#define CLKSEL_Z_PLL 0x03
// values to be selected as DEVICE_RESET
#define DEVICE_RESET 0x80
// end values
