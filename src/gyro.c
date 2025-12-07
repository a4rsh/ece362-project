#include "gyro.h"

const int I2C_GYRO_SDA = 24;
const int I2C_GYRO_SCL = 25;
const int INTERRUPT_GYRO_PIN = 23;

static const uint8_t addr = 0x68;

uint8_t accel_addr = 0x1F;
uint8_t gyro_addr  = 0x25; 

uint8_t config_reg        = 0x11;
uint8_t gyro_config_reg   = 0x4F; 
uint8_t accel_config_reg  = 0x50; 
uint8_t pwr_mgmt0_reg     = 0x4E;

float gyroOffsets[] = {0, 0, 0};
float accelOffsets[] = {0, 0, 0};

float acc_lsb_to_g;
float gyro_lsb_to_degsec;

static float gyro_z_bias_ema = 0.0f;
static const float GYROZ_EMA_ALPHA = 0.5f;
static const float ACCEL_STATIONARY_G_THRESH = 2.0f;
static const float GYRO_STATIONARY_THRESH = 10.0f;

unsigned long preInterval;

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
    setGyroConfig(1);
    setAccConfig(0);

    preInterval = to_ms_since_boot(get_absolute_time());
    getCalibrationOffsets();
}

void gyro_reset()
{

    uint8_t buffer[] = {0x11, 0x01};
    i2c_write_blocking(i2c0, addr, buffer, 2, false);
    sleep_ms(50);


    buffer[0] = pwr_mgmt0_reg; 
    buffer[1] = 0x0F;
    i2c_write_blocking(i2c0, addr, buffer, 2, false);
    sleep_ms(10);
}

int getCalibrationOffsets()
{
    float ag[6] = {0};

    float gyro[3], accel[3];

    for (int i = 0; i < CALIBRATE_SAMPLES; i++)
    {
        int timeout = 0;
        int res;
        do { res = read_accel(accel); timeout++; } while (!res && timeout < 50);
        if (timeout >= 50) return 1;

        timeout = 0;
        do { res = read_gyro(gyro); timeout++; } while (!res && timeout < 50);
        if (timeout >= 50) return 1;

        ag[0] += accel[0];
        ag[1] += accel[1];
        ag[2] += accel[2] - 1.0;

        ag[3] += gyro[0];
        ag[4] += gyro[1];
        ag[5] += gyro[2];

        sleep_ms(1);
    }

    accelOffsets[0] = ag[0] / CALIBRATE_SAMPLES;
    accelOffsets[1] = ag[1] / CALIBRATE_SAMPLES;
    accelOffsets[2] = ag[2] / CALIBRATE_SAMPLES;

    gyroOffsets[0] = ag[3] / CALIBRATE_SAMPLES;
    gyroOffsets[1] = ag[4] / CALIBRATE_SAMPLES;
    gyroOffsets[2] = ag[5] / CALIBRATE_SAMPLES;

    printf("Gyro Offsets: %f, %f, %f\n", gyroOffsets[0], gyroOffsets[1], gyroOffsets[2]);
    printf("Accel Offsets: %f, %f, %f\n", accelOffsets[0], accelOffsets[1], accelOffsets[2]);
    sleep_ms(4000);
    return 0;
}

int updateAngles(float *angleX, float *angleY, float *angleZ)
{
    float gyro[3], accel[3];

    int timeout = 0;
    int res;

    do { res = read_accel(accel); timeout++; } while (!res && timeout < 50);
    if (timeout >= 50) return 1;

    timeout = 0;
    do { res = read_gyro(gyro); timeout++; } while (!res && timeout < 50);
    if (timeout >= 50) return 1;

    float sgZ = accel[2] < 0 ? -1 : 1;

    float angleAccX = atan2(accel[1], sgZ * sqrt(accel[2] * accel[2] + accel[0] * accel[0])) * RAD_2_DEG;
    float angleAccY = -atan2(accel[0], sqrt(accel[2] * accel[2] + accel[1] * accel[1])) * RAD_2_DEG;

    unsigned long Tnew = to_ms_since_boot(get_absolute_time());
    float dt = (Tnew - preInterval) * 1e-3;
    preInterval = Tnew;

    *angleX = wrap(DEFAULT_GYRO_COEF * (angleAccX + wrap(*angleX + gyro[0] * dt - angleAccX, 180)) +
                       (1.0 - DEFAULT_GYRO_COEF) * angleAccX,
                   180);

    *angleY = wrap(DEFAULT_GYRO_COEF * (angleAccY + wrap(*angleY + sgZ * gyro[1] * dt - angleAccY, 90)) +
                       (1.0 - DEFAULT_GYRO_COEF) * angleAccY,
                   90);

    float accel_mag = sqrtf(accel[0] * accel[0] + accel[1] * accel[1] + accel[2] * accel[2]);

    if (fabsf(accel_mag - 1.0f) < ACCEL_STATIONARY_G_THRESH &&
        fabsf(gyro[2]) < GYRO_STATIONARY_THRESH)
    {
        gyro_z_bias_ema = (1.0f - GYROZ_EMA_ALPHA) * gyro_z_bias_ema + GYROZ_EMA_ALPHA * gyro[2];
    }

    float correctedZ = gyro[2] - gyro_z_bias_ema;
    *angleZ += correctedZ * dt;

    return 0;
}

int setGyroConfig(int config_num)
{
    uint8_t fs;
    switch (config_num)
    {
    case 0: gyro_lsb_to_degsec = 131.0; fs = 3; break;
    case 1: gyro_lsb_to_degsec = 65.5;  fs = 2; break;
    case 2: gyro_lsb_to_degsec = 32.8;  fs = 1; break;
    case 3: gyro_lsb_to_degsec = 16.4;  fs = 0; break;
    default: return 1;
    }

    uint8_t buf[2];
    buf[0] = gyro_config_reg;     
    buf[1] = fs << 5;

    int status = i2c_write_blocking(i2c0, addr, buf, 2, false);
    return (status == 2) ? 0 : 1;
}

int setAccConfig(int config_num)
{
    uint8_t fs;
    switch (config_num)
    {
    case 0: acc_lsb_to_g = 16384.0; fs = 3; break;
    case 1: acc_lsb_to_g = 8192.0;  fs = 2; break;
    case 2: acc_lsb_to_g = 4096.0;  fs = 1; break;
    case 3: acc_lsb_to_g = 2048.0;  fs = 0; break;
    default: return 1;
    }

    uint8_t buf[2];
    buf[0] = accel_config_reg;
    buf[1] = fs << 5;

    int status = i2c_write_blocking(i2c0, addr, buf, 2, false);
    return (status == 2) ? 0 : 1;
}

int read_accel(float accel[3])
{
    uint8_t buffer[6];
    int res = i2c_write_blocking(i2c0, addr, &accel_addr, 1, true);
    if (res < 0) return res;

    res = i2c_read_blocking(i2c0, addr, buffer, 6, false);
    if (res < 0) return res;

    for (int i = 0; i < 3; i++)
    {
        int16_t raw = (buffer[i * 2] << 8) | buffer[i * 2 + 1];
        accel[i] = ((float)raw) / acc_lsb_to_g - accelOffsets[i];
    }
    return res;
}

int read_gyro(float gyro[3])
{
    uint8_t buffer[6];
    int res = i2c_write_blocking(i2c0, addr, &gyro_addr, 1, true);
    if (res < 0) return res;

    res = i2c_read_blocking(i2c0, addr, buffer, 6, false);
    if (res < 0) return res;

    for (int i = 0; i < 3; i++)
    {
        int16_t raw = (buffer[i * 2] << 8) | buffer[i * 2 + 1];
        gyro[i] = ((float)raw) / gyro_lsb_to_degsec - gyroOffsets[i];
    }
    return res;
}
