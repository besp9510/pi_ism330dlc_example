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

// Include C standard libraries:
#include <stdlib.h> // C Standard library
#include <stdio.h>  // C Standard I/O libary
#include <time.h>   // C Standard date and time manipulation
#include <stdint.h> // C Standard integer types

// Include user headers:
#include <pi_i2c.h>              // Pi I2C library!
#include <pi_lw_gpio.h>          // Pi GPIO library!
#include <pi_microsleep_hard.h>  // PI microsleep library!

#include "ism330dlc_registers.h" // ISM330DLC register definitions

// To convert from g's to SI units
#define GRAVITY_CONSTANT 9.8066 // [m/s^2]

// Turn the device on and off
#define DEVICE_POWER_GPIO 4 // UPDATE

// Testing ISM330DLC "iNEMO inertial module: 3D accelerometer and 3D gyroscope
// with digital output for industrial applications" per the datasheet:
// (Can find under doc/ism330dlc.pdf)

int i2c_error_handler(int errno) {
    // An I2C error may be fatal or maybe something that we can recover from
    // or even ignore all together:
    switch (errno) {
        case -ENACK:
            printf("I2C Error! Encountered ENACK\n");
            break;
        case -EBADXFR:
            printf("I2C Error! Encountered EBADXFR\n");
            break;
        case -EBADREGADDR:
            printf("I2C Error! Encountered EBADREGADDR\n");
            break;
        case -ECLKTIMEOUT:
            printf("I2C Error! Encountered ECLKTIMEOUT\n");
            break;
        case -ENACKRST:
            printf("I2C Error! Encountered ENACKRST\n");
            break;
        case -EBUSLOCKUP:
            printf("I2C Error! Encountered EBUSLOCKUP\n");
            break;
        case -EBUSUNKERR:
            printf("I2C Error! Encountered EBUSUNKERR\n");
            break;
        case -EFAILSTCOND:
            printf("I2C Error! Encountered EFAILSTCOND\n");
            break;
        case -EDEVICEHUNG:
            printf("I2C Error! Encountered ESLAVEHUNG\n");
            break;
        default:
            break;
    };

    return 0;
}

int scan_for_device(uint8_t device_addr) {
    // Address book passed to returned by the function:
    int address_book[127];

    int ret;

    if ((ret = scan_bus_i2c(address_book)) < 0) {
        i2c_error_handler(ret);
    };

    // Check and see if LIS3MDL was detected on the bus (if not then we can't
    // really continue with the test):
    if (address_book[device_addr] != 1) {
        printf("Device was not detected at 0x%X\n", device_addr);
        return -1;
    };

    printf("Device was detected at 0x%X\n", device_addr);

    return 0;
}


int verify_device_id(uint8_t device_addr) {
    // The returned device ID will be stored into an array that we pass
    // to the read function:
    int device_id[1];

    int ret;

    printf("Verifying device 0x%X identity\n", device_addr);

    if ((ret = read_i2c(device_addr, WHO_AM_I, device_id, 1)) < 0) {
        i2c_error_handler(ret);
    };

    // Compare returned device ID and error out if it does not match expected
    // (If it doesn't match I would suspect something has gone horribly wrong)
    if (device_id[0] != WHO_AM_I_DEFAULT) {
        printf("Device identified as 0x%X but does not match expected 0x%X\n",
               device_id[0], WHO_AM_I_DEFAULT);
        return -1;
    };

    printf("Device identified as 0x%X and matches expected 0x%X\n",
            device_id[0], WHO_AM_I_DEFAULT);

    return 0;
}

// Set device configuration by writing to a register address
int configure_device(int device_addr, int reg_addr, int *configs,
                     int num_configs) {
    int reg_value[1] = {0};

    int i;
    int ret;

    int mask = 0;
    int start_bit = 0;
    int stop_bit = 0;

    printf("Configuring device 0x%X\n", device_addr);

    // Get current register value to apply the options to:
    if ((ret = read_i2c(device_addr, reg_addr, reg_value, 1)) < 0) {
        i2c_error_handler(ret);
        return ret;
    };

    printf("Register 0x%X currently reads 0x%X\n", reg_addr, reg_value[0]);

    // Go through all input options to come up with the final register value:
    for (i = 0; i < num_configs; i++) {
        // AND the current value of the register with a mask to clear the
        // bit of the option being sent then OR it with the option to arrive
        // to the final value of the register:
        stop_bit = configs[i] >> 12;
        start_bit = 0x0F & (configs[i] >> 8);

        mask = ~((2 * ((1 << (stop_bit - start_bit)) - 1) + 1) << start_bit);

        reg_value[0] = (reg_value[0] & mask) | (configs[i] << start_bit);
    }

    printf("Setting register 0x%X to 0x%X\n", reg_addr, (uint8_t) reg_value[0]);

    if ((ret = write_i2c(device_addr, reg_addr, reg_value, 0x01)) < 0) {
        i2c_error_handler(ret);
        return ret;
    };

    printf("Device configured\n");

    return 0;
}

// Get acceleration data and return in mili-g:
int get_accel(int device_addr, int sensitivity, float *accel_data) {
    int raw_accel_data[6] = {0, 0, 0, 0, 0, 0};

    int16_t accel_x_raw = 0;
    int16_t accel_y_raw = 0;
    int16_t accel_z_raw = 0;

    float accel_x_converted = 0;
    float accel_y_converted = 0;
    float accel_z_converted = 0;

    int scale;

    int ret;

    int attempt = 1;

    printf("Getting accelerometer data from device 0x%X\n", device_addr);

    // milli-g/LSB * 1000:
    if (sensitivity == (ACCEL_FS_2_G)) {
        scale = 61;
    } else if (sensitivity == (ACCEL_FS_4_G)) {
        scale = 122;
    } else if (sensitivity == (ACCEL_FS_8_G)) {
        scale = 244;
    } else if (sensitivity == (ACCEL_FS_16_G)) {
        scale = 488;
    };

    // Get raw acceleration data:
    while (attempt) {
        if ((ret = read_i2c(device_addr, OUTX_L_XL, raw_accel_data, 6)) < 0) {
            i2c_error_handler(ret);
            microsleep_hard(1000);
        } else {
            // Break the loop:
            attempt = 0;
        };
    };

    // Get raw acceleration data:
    while (attempt) {
        if ((ret = read_i2c(device_addr, OUTX_L_XL, raw_accel_data, 6)) < 0) {
            i2c_error_handler(ret);
            microsleep_hard(1000);
        } else {
            // Append MSB to LSB:
            accel_x_raw = (raw_accel_data[1] << 8) | raw_accel_data[0];
            accel_y_raw = (raw_accel_data[3] << 8) | raw_accel_data[2];
            accel_z_raw = (raw_accel_data[5] << 8) | raw_accel_data[4];

            // Ensure that all axis reported non-zero values otherwise get
            // more data:
            if ((accel_x_raw != 0) && (accel_y_raw != 0) && (accel_z_raw != 0)) {
                // Break the loop:
                attempt = 0;
            };
        };
    };

    printf("accel_x_raw = %d\n", accel_x_raw);
    printf("accel_y_raw = %d\n", accel_y_raw);
    printf("accel_z_raw = %d\n", accel_z_raw);

    // Convert based on scalar:
    accel_x_converted = (scale * accel_x_raw) / 1000.0f; // [milli-g]
    accel_y_converted = (scale * accel_y_raw) / 1000.0f; // [milli-g]
    accel_z_converted = (scale * accel_z_raw) / 1000.0f; // [milli-g]

    printf("accel_x_converted = %0.3f milli-g\n", accel_x_converted);
    printf("accel_y_converted = %0.3f milli-g\n", accel_y_converted);
    printf("accel_z_converted = %0.3f milli-g\n", accel_z_converted);

    accel_data[0] = accel_x_converted;
    accel_data[1] = accel_y_converted;
    accel_data[2] = accel_z_converted;

    return 0;
}

// Get gyroscope data and return in milli degrees per second:
int get_gyro(int device_addr, int sensitivity, float *gyro_data) {
    int raw_gyro_data[6] = {0, 0, 0, 0, 0, 0};

    int16_t gyro_x_raw = 0;
    int16_t gyro_y_raw = 0;
    int16_t gyro_z_raw = 0;

    float gyro_x_converted = 0;
    float gyro_y_converted = 0;
    float gyro_z_converted = 0;

    int scale;

    int ret;

    int attempt = 1;

    printf("Getting accelerometer data from device 0x%X\n", device_addr);

    // milli-dps/LSB * 1000 / 5:
    if (sensitivity == (GYRO_FS_125_DPS_ENABLED)) {
        scale = 4375 / 5;
    } else if (sensitivity == (GYRO_FS_250_DPS)) {
        scale = 8750 / 5;
    } else if (sensitivity == (GYRO_FS_500_DPS)) {
        scale = 17500 / 5;
    } else if (sensitivity == (GYRO_FS_1000_DPS)) {
        scale = 35000 / 5;
    } else if (sensitivity == (GYRO_FS_2000_DPS)) {
        scale = 70000 / 5;
    };

    printf("scale = %d\n", scale);

    // Get raw gyroscope data:
    while (attempt) {
        if ((ret = read_i2c(device_addr, OUTX_L_G, raw_gyro_data, 6)) < 0) {
            i2c_error_handler(ret);
            microsleep_hard(1000);
        } else {
            // Append MSB to LSB:
            gyro_x_raw = (raw_gyro_data[1] << 8) | raw_gyro_data[0];
            gyro_y_raw = (raw_gyro_data[3] << 8) | raw_gyro_data[2];
            gyro_z_raw = (raw_gyro_data[5] << 8) | raw_gyro_data[4];

            // Ensure that all axis reported non-zero values otherwise get
            // more data:
            if ((gyro_x_raw != 0) && (gyro_y_raw != 0) && (gyro_z_raw != 0)) {
                // Break the loop:
                attempt = 0;
            };
        };
    };

    printf("gyro_x_raw = %d\n", gyro_x_raw);
    printf("gyro_y_raw = %d\n", gyro_y_raw);
    printf("gyro_z_raw = %d\n", gyro_z_raw);

    // Convert based on scalar:
    gyro_x_converted = (5 * scale * gyro_x_raw) / 1000.0f; // [milli-dps]
    gyro_y_converted = (5 * scale * gyro_y_raw) / 1000.0f; // [milli-dps]
    gyro_z_converted = (5 * scale * gyro_z_raw) / 1000.0f; // [milli-dps]

    printf("gyro_x_converted = %0.3f milli-dps\n", gyro_x_converted);
    printf("gyro_y_converted = %0.3f milli-dps\n", gyro_y_converted);
    printf("gyro_z_converted = %0.3f milli-dps\n", gyro_z_converted);

    gyro_data[0] = gyro_x_converted;
    gyro_data[1] = gyro_y_converted;
    gyro_data[2] = gyro_z_converted;

    return 0;
}

int main(void) {
    // ISM330DLC slave address (page 17):
    uint8_t ism330dlc_addr = 0x6A;

    // Use the default I2C pins:
    // Ensure that Raspian I2C interface is disabled via rasp-config otherwise
    // risk unpredictable behavior!
    int sda_pin = 2; // UPDATE
    int scl_pin = 3; // UPDATE

    int speed_grade = I2C_FULL_SPEED;

    int config[7];

    int ret;
    int i;

    // CSV file to save data:
    FILE* fpt;

    static int number_of_samples = 500;

    float accel_data[3];
    float gyro_data[3];

    float accel_x[number_of_samples];
    float accel_y[number_of_samples];
    float accel_z[number_of_samples];

    float gyro_x[number_of_samples];
    float gyro_y[number_of_samples];
    float gyro_z[number_of_samples];

    struct timespec sample_time;

    long int sample_second[number_of_samples];
    long int sample_nano_second[number_of_samples];

    printf("Begin test_ism330dlc.c\n");
    printf("Configuring pi_i2c:\n");
    printf("sda_pin = %d\n", sda_pin);
    printf("sda_pin = %d\n", scl_pin);
    printf("speed_grade = %d Hz\n", speed_grade);

    // Turn on the PCA9685:
    gpio_set_mode(GPIO_OUTPUT, DEVICE_POWER_GPIO);
    gpio_set(DEVICE_POWER_GPIO);

    printf("ISM330DLC turned on\n");

    // Configure at standard mode:
    if ((ret = config_i2c(sda_pin, scl_pin, speed_grade)) < 0 ) {
        printf("config_i2c() failed to configure and returned %d\n", ret);
        return ret;
    };

    // Check to see if the device is present prior to interacting with device:
    if ((ret = scan_for_device(ism330dlc_addr)) < 0) {
        return -1;
    };

    // Check to see if the device ID matches what's expected prior to
    // continuing with the test:
    if ((ret = verify_device_id(ism330dlc_addr)) < 0) {
        return -1;
    };

    // Configure FIFO:
    // - Bypass mode. FIFO disabled
    config[0] = FIFO_BYPASS_MODE;

    if ((ret = configure_device(ism330dlc_addr, FIFO_CTRL5, config, 1)) < 0) {
        return ret;
    };

    // Configure accelerometer:
    // - 56 Hz sampling rate
    // - Full-scale of plus minus 2Gs
    config[0] = ACCEL_52_HZ; config[1] = ACCEL_FS_2_G;

    if ((ret = configure_device(ism330dlc_addr, CTRL1_XL, config, 2)) < 0) {
        return ret;
    };

    // Configure gyroscope:
    // - 56 Hz sampling rate
    // - Full-scale of 250 degrees per second
    config[0] = GYRO_52_HZ; config[1] = GYRO_FS_250_DPS;

    if ((ret = configure_device(ism330dlc_addr, CTRL2_G, config, 2)) < 0) {
        return ret;
    };

    sleep(1);

    printf("Getting accelerometer gyroscope data\n");

    // Get a number of samples:
    for (i = 0; i < number_of_samples; i++) {
        // Get accelerometer data:
        if ((ret = get_accel(ism330dlc_addr, ACCEL_FS_2_G, accel_data)) < 0) {
            return ret;
        };

        // Get gyroscope data:
        if ((ret = get_gyro(ism330dlc_addr, GYRO_FS_250_DPS, gyro_data)) < 0) {
            return ret;
        };

        // Grab the time:
        clock_gettime(CLOCK_MONOTONIC, &sample_time);

        // Save off data for later:
        sample_second[i] = sample_time.tv_sec;
        sample_nano_second[i] = sample_time.tv_nsec;

        accel_x[i] = accel_data[0];
        accel_y[i] = accel_data[1];
        accel_z[i] = accel_data[2];

        gyro_x[i] = gyro_data[0];
        gyro_y[i] = gyro_data[1];
        gyro_z[i] = gyro_data[2];

        microsleep_hard(0.05e6);
    };

    printf("Finished test\n");
    printf("Writing results to test_ism330dlc.csv\n");

    // Create a CSV file and write data to it
    fpt = fopen("test_ism330dlc.csv", "w+");

    fprintf(fpt,"Sample Timestamp, Acceleration X, Acceleration Y,"\
            " Acceleration Z, Gyroscope X, Gyroscope Y, Gyroscope Z\n");

    for (i = 0; i < number_of_samples; i++) {
        fprintf(fpt,"%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n",
                sample_second[i] + sample_nano_second[i] * 1e-9, accel_x[i],
                accel_y[i], accel_z[i], gyro_x[i], gyro_y[i], gyro_z[i]);
    };

    // Done writing so let's close it:
    fclose(fpt);

    // Turn off the PCA9685:
    gpio_clear(DEVICE_POWER_GPIO);

    printf("ISM330DLC turned off\n");

    return 0;
}
