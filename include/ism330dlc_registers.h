// Raspberry Pi ISM330DLC Example
//
// Copyright (c) 2022 Benjamin Spencer
// ============================================================================
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// ============================================================================

// ISM330DLC register address, defaults, and masks from data sheet

// Register addresses (page 38 to 40):
#define FUNC_CFG_ACCESS 0x01          // Embedded functions config register
#define SENSOR_SYNC_TIME_FRAME 0x04   //
#define SENSOR_SYNC_RES_RATIO 0x05    //
#define FIFO_CTRL1 0x06               // FIFO configuration registers
#define FIFO_CTRL2 0x07               // FIFO configuration registers
#define FIFO_CTRL3 0x08               // FIFO configuration registers
#define FIFO_CTRL4 0x09               // FIFO configuration registers
#define FIFO_CTRL5 0x0A               // FIFO configuration registers
#define DRDY_PULSE_CFG 0x0B           //
#define INT1_CTRL 0x0D                // INT1 pin control
#define INT2_CTRL 0x0E                // INT2 pin control
#define WHO_AM_I 0x0F                 // Device identification register
#define CTRL1_XL 0x10                 // Accel and gyroscope control registers
#define CTRL2_G 0x11                  // Accel and gyroscope control registers
#define CTRL3_C 0x12                  // Accel and gyroscope control registers
#define CTRL4_C 0x13                  // Accel and gyroscope control registers
#define CTRL5_C 0x14                  // Accel and gyroscope control registers
#define CTRL6_C 0x15                  // Accel and gyroscope control registers
#define CTRL7_G 0x16                  // Accel and gyroscope control registers
#define CTRL8_XL 0x17                 // Accel and gyroscope control registers
#define CTRL9_XL 0x18                 // Accel and gyroscope control registers
#define CTRL10_C 0x19                 // Accel and gyroscope control registers
#define MASTER_CONFIG 0x1A            // I2C master configuration register
#define WAKE_UP_SRC 0x1B              // Interrupt registers
#define TAP_SRC 0x1C                  // Interrupt registers
#define D6D_SRC 0x1D                  // Interrupt registers
#define STATUS_SPIAux 0x1E            // Status data reg for GP and OIS data
#define OUT_TEMP_L 0x20               // Temperature output data registers
#define OUT_TEMP_H 0x21               // Temperature output data registers
#define OUTX_L_G 0x22                 // Gyro output reg for GP and OIS data
#define OUTX_H_G 0x23                 // Gyro output reg for GP and OIS data
#define OUTY_L_G 0x24                 // Gyro output reg for GP and OIS data
#define OUTY_H_G 0x25                 // Gyro output reg for GP and OIS data
#define OUTZ_L_G 0x26                 // Gyro output reg for GP and OIS data
#define OUTZ_H_G 0x27                 // Gyro output reg for GP and OIS data
#define OUTX_L_XL 0x28                // Accel output reg for GP and OIS data
#define OUTX_H_XL 0x29                // Accel output reg for GP and OIS data
#define OUTY_L_XL 0x2A                // Accel output reg for GP and OIS data
#define OUTY_H_XL 0x2B                // Accel output reg for GP and OIS data
#define OUTZ_L_XL 0x2C                // Accel output reg for GP and OIS data
#define OUTZ_H_XL 0x2D                // Accel output reg for GP and OIS data
#define SENSORHUB1_REG 0x2E           // Accelerometer output registers
#define SENSORHUB2_REG 0x2F           // Accelerometer output registers
#define SENSORHUB3_REG 0x30           // Accelerometer output registers
#define SENSORHUB4_REG 0x31           // Accelerometer output registers
#define SENSORHUB5_REG 0x32           // Accelerometer output registers
#define SENSORHUB6_REG 0x33           // Accelerometer output registers
#define SENSORHUB7_REG 0x34           // Accelerometer output registers
#define SENSORHUB8_REG 0x35           // Accelerometer output registers
#define SENSORHUB9_REG 0x36           // Accelerometer output registers
#define SENSORHUB10_REG 0x37          // Accelerometer output registers
#define SENSORHUB11_REG 0x38          // Accelerometer output registers
#define SENSORHUB12_REG 0x39          // Accelerometer output registers
#define FIFO_STATUS1 0x3A             // FIFO status registers
#define FIFO_STATUS2 0x3B             // FIFO status registers
#define FIFO_STATUS3 0x3C             // FIFO status registers
#define FIFO_STATUS4 0x3D             // FIFO status registers
#define FIFO_DATA_OUT_L 0x3E          // FIFO data output registers
#define FIFO_DATA_OUT_H 0x3F          // FIFO data output registers
#define TIMESTAMP0_REG 0x40           // Timestamp output registers
#define TIMESTAMP1_REG 0x41           // Timestamp output registers
#define TIMESTAMP2_REG 0x42           // Timestamp output registers
#define SENSORHUB13_REG 0x4D          // Sensor hub output registers
#define SENSORHUB14_REG 0x4E          // Sensor hub output registers
#define SENSORHUB15_REG 0x4F          // Sensor hub output registers
#define SENSORHUB16_REG 0x50          // Sensor hub output registers
#define SENSORHUB17_REG 0x51          // Sensor hub output registers
#define SENSORHUB18_REG 0x52          // Sensor hub output registers
#define FUNC_SRC1 0x53                // Interrupt registers
#define FUNC_SRC2 0x54                // Interrupt registers
#define TAP_CFG 0x58                  // Interrupt registers
#define TAP_THS_6D 0x59               // Interrupt registers
#define INT_DUR2 0x5A                 // Interrupt registers
#define WAKE_UP_THS 0x5B              // Interrupt registers
#define WAKE_UP_DUR 0x5C              // Interrupt registers
#define FREE_FALL 0x5D                // Interrupt registers
#define MD1_CFG 0x5E                  // Interrupt registers
#define MD2_CFG 0x5F                  // Interrupt registers
#define MASTER_CMD_CODE 0x60          // Interrupt registers
#define SENS_SYNC_SPI_ERROR_CODE 0x61 //
#define OUT_MAG_RAW_X_L 0x66          // External mag raw data output reg
#define OUT_MAG_RAW_X_H 0x67          // External mag raw data output reg
#define OUT_MAG_RAW_Y_L 0x68          // External mag raw data output reg
#define OUT_MAG_RAW_Y_H 0x69          // External mag raw data output reg
#define OUT_MAG_RAW_Z_L 0x6A          // External mag raw data output reg
#define OUT_MAG_RAW_Z_H 0x6B          // External mag raw data output reg
#define INT_OIS 0x6F                  //
#define CTRL1_OIS 0x70                // Control registers for OIS connection
#define CTRL2_OIS 0x71                // Control registers for OIS connection
#define CTRL3_OIS 0x72                // Control registers for OIS connection
#define X_OFS_USR 0x73                // Accelerometer user offset correction
#define Y_OFS_USR 0x74                // Accelerometer user offset correction
#define Z_OFS_USR 0x75                // Accelerometer user offset correction

// Register defaults (page 38 to 40):
#define FUNC_CFG_ACCESS_DEFAULT 0x00         // (= 00000000)
#define SENSOR_SYNC_TIME_FRAME_DEFAULT 0x00  // (= 00000000)
#define SENSOR_SYNC_RES_RATIO_DEFAULT 0x00   // (= 00000000)
#define FIFO_CTRL1_DEFAULT 0x00              // (= 00000000)
#define FIFO_CTRL2_DEFAULT 0x00              // (= 00000000)
#define FIFO_CTRL3_DEFAULT 0x00              // (= 00000000)
#define FIFO_CTRL4_DEFAULT 0x00              // (= 00000000)
#define FIFO_CTRL5_DEFAULT 0x00              // (= 00000000)
#define DRDY_PULSE_CFG_DEFAULT 0x00          // (= 00000000)
#define INT1_CTRL_DEFAULT 0x00               // (= 00000000)
#define INT2_CTRL_DEFAULT 0x00               // (= 00000000)
#define WHO_AM_I_DEFAULT 0x6B                // (= 01101010)
#define CTRL1_XL_DEFAULT 0x00                // (= 00000000)
#define CTRL2_G_DEFAULT 0x00                 // (= 00000000)
#define CTRL3_C_DEFAULT 0x04                 // (= 00000100)
#define CTRL4_C_DEFAULT 0x00                 // (= 00000000)
#define CTRL5_C_DEFAULT 0x00                 // (= 00000000)
#define CTRL6_C_DEFAULT 0x00                 // (= 00000000)
#define CTRL7_G_DEFAULT 0x00                 // (= 00000000)
#define CTRL8_XL_DEFAULT 0x00                // (= 00000000)
#define CTRL9_XL_DEFAULT 0xE0                // (= 11100000)
#define CTRL10_C_DEFAULT 0x00                // (= 00000000)
#define MASTER_CONFIG_DEFAULT 0x00           // (= 00000000)
#define TAP_CFG_DEFAULT 0x00                 // (= 00000000)
#define TAP_THS_6D_DEFAULT 0x00              // (= 00000000)
#define INT_DUR2_DEFAULT 0x00                // (= 00000000)
#define WAKE_UP_THS_DEFAULT 0x00             // (= 00000000)
#define WAKE_UP_DUR_DEFAULT 0x00             // (= 00000000)
#define FREE_FALL_DEFAULT 0x00               // (= 00000000)
#define MD1_CFG_DEFAULT 0x00                 // (= 00000000)
#define MD2_CFG_DEFAULT 0x00                 // (= 00000000)
#define MASTER_CMD_CODE_DEFAULT 0x00         // (= 00000000)
#define SENS_SYNC_SPI_ERROR_CODE_DEFAULT 0x0 // (= 00000000)
#define INT_OIS_DEFAULT 0x00                 // (= 00000000)
#define CTRL1_OIS_DEFAULT 0x00               // (= 00000000)
#define CTRL2_OIS_DEFAULT 0x00               // (= 00000000)
#define CTRL3_OIS_DEFAULT 0x00               // (= 00000000)
#define X_OFS_USR_DEFAULT 0x00               // (= 00000000)
#define Y_OFS_USR_DEFAULT 0x00               // (= 00000000)
#define Z_OFS_USR_DEFAULT 0x00               // (= 00000000)

// Register setting masks (page 41-85):
// (OR the starting bit location of the setting at the end of the byte)
// (OR the stoping bit location of the setting at the end of the byte)
#define FIFO_BYPASS_MODE 0x00 | (0x00 << 8) | (0x02 << 12)
#define FIFO_FIFO_MODE 0x01| (0x00 << 8) | (0x02 << 12)
#define FIFO_CONTINUOUS_FIFO_MODE 0x03 | (0x00 << 8) | (0x02 << 12)
#define FIFO_BYPASS_CONTINUOUS_MODE 0x04 | (0x00 << 8) | (0x02 << 12)
#define FIFO_CONTINUOUS_MODE 0x06 | (0x00 << 8) | (0x02 << 12)

#define ACCEL_FS_2_G 0x00 | (0x02 << 8) | (0x03 << 12)
#define ACCEL_FS_4_G 0x02 | (0x02 << 8) | (0x03 << 12)
#define ACCEL_FS_8_G 0x03 | (0x02 << 8) | (0x03 << 12)
#define ACCEL_FS_16_G 0x01 | (0x02 << 8) | (0x03 << 12)

#define ACCEL_POWER_DOWN 0x00 | (0x04 << 8) | (0x07 << 12)
#define ACCEL_1_DOT_6_HZ 0x0B | (0x04 << 8) | (0x07 << 12)
#define ACCEL_12_DOT_5_HZ 0x01 | (0x04 << 8) | (0x07 << 12)
#define ACCEL_52_HZ 0x03 | (0x04 << 8) | (0x07 << 12)
#define ACCEL_104_HZ 0x04 | (0x04 << 8) | (0x07 << 12)
#define ACCEL_208_HZ 0x05 | (0x04 << 8) | (0x07 << 12)
#define ACCEL_416_HZ 0x06 | (0x04 << 8) | (0x07 << 12)
#define ACCEL_833_HZ 0x07 | (0x04 << 8) | (0x07 << 12)
#define ACCEL_1_DOT_66_K_HZ 0x08 | (0x04 << 8) | (0x07 << 12)
#define ACCEL_3_DOT_33_K_HZ 0x09 | (0x04 << 8) | (0x07 << 12)
#define ACCEL_6_DOT_66_K_HZ 0x0A | (0x04 << 8) | (0x07 << 12)

#define GYRO_FS_125_DPS_ENABLED 0x01 | (0x01 << 8) | (0x01 << 12)
#define GYRO_FS_125_DPS_DISABLED 0x00 | (0x01 << 8) | (0x01 << 12)
#define GYRO_FS_250_DPS 0x00 | (0x02 << 8) | (0x03 << 12)
#define GYRO_FS_500_DPS 0x01 | (0x02 << 8) | (0x03 << 12)
#define GYRO_FS_1000_DPS 0x02 | (0x02 << 8) | (0x03 << 12)
#define GYRO_FS_2000_DPS 0x03 | (0x02 << 8) | (0x03 << 12)

#define GYRO_POWER_DOWN 0x00 | (0x04 << 8) | (0x07 << 12)
#define GYRO_12_DOT_5_HZ 0x01 | (0x04 << 8) | (0x07 << 12)
#define GYRO_26_HZ 0x02 | (0x04 << 8) | (0x07 << 12)
#define GYRO_52_HZ 0x03 | (0x04 << 8) | (0x07 << 12)
#define GYRO_104_HZ 0x04 | (0x04 << 8) | (0x07 << 12)
#define GYRO_208_HZ 0x05 | (0x04 << 8) | (0x07 << 12)
#define GYRO_416_HZ 0x06 | (0x04 << 8) | (0x07 << 12)
#define GYRO_833_HZ 0x07 | (0x04 << 8) | (0x07 << 12)
#define GYRO_1_DOT_66_K_HZ 0x08 | (0x04 << 8) | (0x07 << 12)
#define GYRO_3_DOT_33_K_HZ 0x09 | (0x04 << 8) | (0x07 << 12)
#define GYRO_6_DOT_66_K_HZ 0x0A | (0x04 << 8) | (0x07 << 12)

#define GYRO_FTYPE_0 0x00 | (0x00 << 8) | (0x01 << 12)
#define GYRO_FTYPE_1 0x01 | (0x00 << 8) | (0x01 << 12)
#define GYRO_FTYPE_2 0x02 | (0x00 << 8) | (0x01 << 12)
#define GYRO_FTYPE_3 0x03 | (0x00 << 8) | (0x01 << 12)

#define LPF1_SEL_G_ENABLED 0x01 | (0x01 << 8) | (0x01 << 12)
#define LPF1_SEL_G_DISABLED 0x00 | (0x01 << 8) | (0x01 << 12)

#define G_HM_MODE_ENABLED 0x00 | (0x07 << 8) | (0x07 << 12)
#define G_HM_MODE_DISABLED 0x01 | (0x07 << 8) | (0x07 << 12)