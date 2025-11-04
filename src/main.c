#include "stdio.h"
#include "pico/stdlib.h"
#include "gyro.h"

int main()
{
    stdio_init_all();

    gyro_init();

    int res;
    float angleX, angleY, angleZ;
    float gyro[3];


    for (;;)
    {
        res = updateAngles(&angleX, &angleY, &angleZ);
        // res = read_gyro(gyro);
        if (res < 0)
        {
            printf("Read failed, %d\n", res);
        }
        else
        {
            printf("Gyro Angles: %0.3f, %0.3f, %0.3f\n", angleX, angleY, angleZ);
        }
        sleep_ms(1000);
    }
}