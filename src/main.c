#include "stdio.h"
#include "pico/stdlib.h"
#include "gyro.h"

int main()
{
    stdio_init_all();

    gyro_init();
    int16_t gyro[3];
    int res;

    for (;;)
    {
        res = read_gyro(gyro);
        if (res < 0)
        {
            printf("Read failed, %d\n", res);
        }
        else
        {
            printf("Gyro Values: %d, %d, %d\n", gyro[0], gyro[1], gyro[2]);
        }
        sleep_ms(100);
    }
}