
#include "gyro.h"

#define CALIBRATE_SAMPLES 500

const int I2C_GYRO_SDA = 28;
const int I2C_GYRO_SCL = 29;

static int addr = 0x68;

uint8_t gyro_addr = 0x43;
uint8_t accel_addr = 0x3B;

float gyroOffsets[] = {0, 0, 0};
float accelOffsets[] = {0, 0, 0};

float acc_to_g = 16384.0;
float gyro_to_degsec = 131.0;

// Gyro and acceleration data buffers

void gyro_init()
{
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(I2C_GYRO_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_GYRO_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_GYRO_SDA);
    gpio_pull_up(I2C_GYRO_SCL);
    bi_decl(bi_2pins_with_func(I2C_GYRO_SDA, I2C_GYRO_SCL, GPIO_FUNC_I2C));
    gyro_reset();
}

void gyro_reset()
{
    uint8_t buffer[] = {0x6B, 0x80};
    i2c_write_blocking(i2c0, addr, buffer, 2, false);
    sleep_ms(100);

    buffer[1] = 0x00;
    i2c_write_blocking(i2c0, addr, buffer, 2, false);
    sleep_ms(10);
}

// Calibration for gyro sensor
// MUST BE STILL AND IN A NEUTRAL POSITION WHEN RUN
// Done once at the beginning of game - no need to put on separate CPU core
int getCalibrationOffsets()
{
    float ag[6] = {0, 0, 0, 0, 0, 0};

    // Temporary buffers for gyro and accel data
    float gyro[3], accel[3];

    for (int i = 0; i < CALIBRATE_SAMPLES; i++)
    {
        // Get data from accelerometer and gyroscope with 50 cycle timeout
        int timeout = 0;
        int res;
        do
        {
            res = read_accel(accel);
            timeout++;
        } while (!res && timeout < 50);

        if (timeout >= 50) // ERROR could not read
            return 1;

        timeout = 0;
        do
        {
            res = read_gyro(gyro);
            timeout++;
        } while (!res && timeout < 50);

        if (timeout >= 50) // ERROR could not read
            return 1;

        // Accumulate the values
        ag[0] += accel[0];
        ag[1] += accel[1];
        ag[2] += accel[2] - 1.0;

        ag[3] += gyro[0];
        ag[4] += gyro[1];
        ag[5] += gyro[2];

        // Delay to not get stale values
        sleep_ms(1);
    }

    // Get offsets by dividing by number of samples
    accelOffsets[0] = ag[0] / CALIBRATE_SAMPLES;
    accelOffsets[1] = ag[1] / CALIBRATE_SAMPLES;
    accelOffsets[2] = ag[2] / CALIBRATE_SAMPLES;

    gyroOffsets[0] = ag[3] / CALIBRATE_SAMPLES;
    gyroOffsets[1] = ag[4] / CALIBRATE_SAMPLES;
    gyroOffsets[2] = ag[5] / CALIBRATE_SAMPLES;

    return 0;
}

int updateAngles(float *angleX, float *angleY, float *angleZ)
{

    return 0;
}

// Need to implement this with non-blocking / another cpu core
int read_accel(float accel[3])
{
    uint8_t buffer[6];

    int res = i2c_write_blocking(i2c0, addr, &accel_addr, 1, true);
    if (res < 0)
        return res;
    res = i2c_read_blocking(i2c0, addr, buffer, 6, false);
    if (res < 0)
        return res;

    for (int i = 0; i < 3; i++)
    {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]) / acc_to_g - accelOffsets[i];
    }
    return res;
}

// Need to implement this with non-blocking / another cpu core
int read_gyro(float gyro[3])
{
    uint8_t buffer[6];

    int res = i2c_write_blocking(i2c0, addr, &gyro_addr, 1, true);
    if (res < 0)
        return res;
    res = i2c_read_blocking(i2c0, addr, buffer, 6, false);
    if (res < 0)
        return res;

    for (int i = 0; i < 3; i++)
    {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]) / gyro_to_degsec - gyroOffsets[i];
    }
    return res;
}