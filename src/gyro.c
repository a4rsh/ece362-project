#include "gyro.h"

const int I2C_GYRO_SDA = 28;
const int I2C_GYRO_SCL = 29;
const int INTERRUPT_GYRO_PIN; //TODO

extern int16_t gyro[3];

static int addr = 0x68;

void gyro_init() {
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
}

void gyro_reset() {
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