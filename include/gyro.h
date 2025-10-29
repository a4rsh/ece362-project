#ifndef GYRO_H
#define GYRO_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"

void gyro_init();
static void gyro_reset();
int read_gyro(int16_t gyro[3]);

#endif