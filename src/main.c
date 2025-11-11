#include "stdio.h"
#include "pico/stdlib.h"
#include "gyro.h"
#include "audio.h"

void test_gyro(void);
void test_audio(void);

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    // test_gyro();
    test_audio();
}

void test_audio(void)
{

    audio_init();

    audio_play("car.wav", 128, false);

    for (;;)
    {
        tight_loop_contents();
    }
}

void test_gyro(void)
{
    printf("INITIALIZING\n");
    gyro_init();
    printf("DONE\n");

    int res;
    float angleX = 0, angleY = 0, angleZ = 0;

    for (;;)
    {
        res = updateAngles(&angleX, &angleY, &angleZ);
        if (res < 0)
        {
            printf("Read failed, %d\n", res);
        }
        else
        {
            printf("Gyro Angles: %0.3f, %0.3f, %0.3f\n", angleX, angleY, angleZ);
        }
        sleep_ms(100);
    }
}
