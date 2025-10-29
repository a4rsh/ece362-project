#include "gyro.h"

const int I2C_GYRO_SDA = 28;
const int I2C_GYRO_SCL = 29;

static int addr = 0x68;

void gyro_init() {
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(I2C_GYRO_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_GYRO_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_GYRO_SDA);
    gpio_pull_up(I2C_GYRO_SCL);
    bi_decl(bi_2pins_with_func(I2C_GYRO_SDA, I2C_GYRO_SCL, GPIO_FUNC_I2C));
    gyro_reset();
}

void gyro_reset() {
    uint8_t buffer[] = {0x6B, 0x80};
    i2c_write_blocking(i2c0, addr, buffer, 2, false);
    sleep_ms(100);

    buffer[1] = 0x00;
    i2c_write_blocking(i2c0, addr, buffer, 2, false); 
    sleep_ms(10);
}
    
int read_gyro(int16_t gyro[3]) {
    uint8_t buffer[6];
    uint8_t val = 0x43;
    
    int res = i2c_write_blocking(i2c0, addr, &val, 1, true);
    if (res < 0) return res;
    res = i2c_read_blocking(i2c0, addr, buffer, 6, false);
    if (res < 0) return res;

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }
    return res;
}