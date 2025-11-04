
#include "gyro.h"

const int I2C_GYRO_SDA = 28;
const int I2C_GYRO_SCL = 29;
const int INTERRUPT_GYRO_PIN; //TODO

extern int16_t gyro[3];

static int addr = 0x68;

uint8_t gyro_addr = 0x43;
uint8_t accel_addr = 0x3B;

float gyroOffsets[] = {0, 0, 0};
float accelOffsets[] = {0, 0, 0};

float acc_to_g = 16384.0;
float gyro_to_degsec = 131.0;

unsigned long preInterval;

// Gyro and acceleration data buffers

static float wrap(float angle, float limit)
{
    while (angle > limit)
        angle -= 2 * limit;
    while (angle < -limit)
        angle += 2 * limit;
    return angle;
}

void gyro_init()
{
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(I2C_GYRO_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_GYRO_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_GYRO_SDA);
    gpio_pull_up(I2C_GYRO_SCL);
    bi_decl(bi_2pins_with_func(I2C_GYRO_SDA, I2C_GYRO_SCL, GPIO_FUNC_I2C));
    gyro_reset();
    gyro_intr_init();
}

void gyro_intr_init() {
    uint8_t buffer[] = {0x38, 0x01};
    i2c_write_timeout_us(i2c0, addr, buffer, 2, false, 0);
    gpio_add_raw_irq_handler(INTERRUPT_GYRO_PIN, read_gyro);
    gpio_set_irq_enabled(INTERRUPT_GYRO_PIN, GPIO_IRQ_EDGE_RISE, true);
    irq_set_enabled(IO_IRQ_BANK0, true);

    preInterval = to_ms_since_boot(get_absolute_time());
}

void gyro_reset()
{
    uint8_t buffer[] = {0x6B, 0x80};
    i2c_write_timeout_us(i2c0, addr, buffer, 2, false, 0);    
    sleep_ms(100);

    buffer[1] = 0x00;
    i2c_write_timeout_us(i2c0, addr, buffer, 2, false, 0);
    sleep_ms(10);
}
    
void read_gyro() {
    uint8_t buffer[6];
    uint8_t val = 0x43;

    gpio_acknowledge_irq(INTERRUPT_GYRO_PIN, gpio_get_irq_event_mask(INTERRUPT_GYRO_PIN) & GPIO_IRQ_EDGE_RISE);
    
    int res = i2c_write_timeout_us(i2c0, addr, &val, 1, true, 0);
    if (res < 0) return res;
    i2c_read_raw_blocking(i2c0, buffer, 6);

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }
    printf("Gyro Values: %d, %d, %d\n", gyro[0], gyro[1], gyro[2]);
    // return res;
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
    float gyro[3], accel[3];

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

    float sgZ = accel[2] < 0 ? -1 : 1;
    float angleAccX = atan2(accel[1], sgZ * sqrt(accel[2] * accel[2] + accel[0] * accel[0])) * RAD_2_DEG;
    float angleAccY = -atan2(accel[0], sqrt(accel[2] * accel[2] + accel[1] * accel[1])) * RAD_2_DEG;

    unsigned long Tnew = to_ms_since_boot(get_absolute_time());
    float dt = (Tnew - preInterval) * 1e-3;
    preInterval = Tnew;

    // Wrap x and y angles - from https://github.com/gabriel-milan/TinyMPU6050/issues/6

    *angleX = wrap(DEFAULT_GYRO_COEF * (angleAccX + wrap(*angleX + gyro[0] * dt - angleAccX, 180)) + (1.0 - DEFAULT_GYRO_COEF) * angleAccX, 180);
    *angleY = wrap(DEFAULT_GYRO_COEF * (angleAccY + wrap(*angleY + sgZ * gyro[1] * dt - angleAccY, 90)) + (1.0 - DEFAULT_GYRO_COEF) * angleAccY, 90);
    *angleZ += gyro[2] * dt; // not wrapped

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

// // Need to implement this with non-blocking / another cpu core
// int read_gyro(float gyro[3])
// {
//     uint8_t buffer[6];

//     int res = i2c_write_blocking(i2c0, addr, &gyro_addr, 1, true);
//     if (res < 0)
//         return res;
//     res = i2c_read_blocking(i2c0, addr, buffer, 6, false);
//     if (res < 0)
//         return res;

//     for (int i = 0; i < 3; i++)
//     {
//         gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]) / gyro_to_degsec - gyroOffsets[i];
//     }
//     printf("Gyro Values: %d, %d, %d\n", gyro[0], gyro[1], gyro[2]);
//     // return res;
// }