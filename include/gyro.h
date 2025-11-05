#ifndef GYRO_H
#define GYRO_H

#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/float.h"
#define RAD_2_DEG 57.29578
#define CALIBRATE_SAMPLES 500
#define DEFAULT_GYRO_COEF 0.98

void gyro_init();
void gyro_reset();
int read_gyro();
void gyro_intr_init();
int getCalibrationOffsets();
int read_accel(float accel[3]);
int getCalibrationOffsets();
int updateAngles(float *angleX, float *angleY, float *angleZ);

#endif