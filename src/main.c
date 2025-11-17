#include "stdio.h"
#include "pico/stdlib.h"
#include "gyro.h"
#include "audio.h"

void vga_init();
void update_graphics(float angleX, float angleY, float angleZ);

void test_gyro(void);
void test_audio(void);

void graphics_demo();

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    // test_gyro();
    // test_audio();
    // test_vga();
    graphics_demo();

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

void graphics_demo(void) 
{
    gyro_init();
    vga_init();

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
            update_graphics(angleX, angleY, angleZ);
        }
        sleep_ms(10);
    }
}
