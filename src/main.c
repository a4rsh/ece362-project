#include "stdio.h"
#include "pico/stdlib.h"
#include "gyro.h"
#include "audio.h"
#include "game_dot.h"
#include <stdlib.h>
#include "game.h"

void update_graphics(float angleX, float angleY, float angleZ, GameDot *dot1);

void test_gyro(void);
void test_audio(void);

void start_game();


int main()
{
    stdio_init_all();
    sleep_ms(2000);

    // test_gyro();
    // test_audio();
    // test_vga();

    start_game();

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

void start_game()
{
    gyro_init();
    vga_init();

    int res;
    float angleX = 0, angleY = 0, angleZ = 0;

    //This is low key unneccesary but I wanted to use X macros because I read about them
    #define X(val) val,
    const int gear_speeds[] = {
        GEAR_SPEEDS
    };
    #undef Xs

    printf("Speed gets hard set to gear speed 1 = %d\n", gear_speeds[1]);

    int speed = gear_speeds[1];
    double steer;
    int curve_timer;
    float curve_target;
    float road_curve;
    int divider_offset;
    double player_x;
    int* SYST_CVR = (int*) 0x0e018;
    int delta;
    int colorFlag = 0;

    int roadBuffer[NUM_SEGMENTS];

    draw_bg();

    srand(*SYST_CVR); //This is most likely wrong. And it's the reason everything breaks.

    // for (;;)
    // {
    //     res = updateAngles(&angleX, &angleY, &angleZ);
    //     if (res < 0)
    //     {
    //         printf("Read failed, %d\n", res);
    //         continue;
    //     }

    //     //Steering
    //     steer = angleY/180;
    //     player_x += steer * TURN_RATE * speed;

    //     //Random curvature
    //     #define MINWAIT 50
    //     #define MAXWAIT 400

    //     #define MINCURVE 10
    //     #define MAXCURVE 90
        
    //     curve_timer--;
    //     if (curve_timer <= 0) {
    //         curve_timer = MINWAIT + (rand() % (MAXWAIT - MINWAIT + 1));
    //         curve_target = MINCURVE + (rand() % (MAXCURVE - MINCURVE + 1));
    //     }
    //     road_curve += (curve_target - road_curve)/2;

    //     //Update road buffer
    //     divider_offset += speed;
    //     divider_offset %= 10;
    //     delta = roadBuffer[1] - roadBuffer[0];

    //     for(int i = 0; i < NUM_SEGMENTS - speed; i++) {
    //         roadBuffer[i] = roadBuffer[i + speed] - delta;
    //     }

    //     roadBuffer[0] = 0; //For our own sanity
    //     player_x -= delta * 3;
        
    //     roadBuffer[NUM_SEGMENTS - 1] = roadBuffer[NUM_SEGMENTS - 2] + road_curve;

    //     colorFlag ^= (speed % 2); //Update color in each row

    //     if(abs(player_x) > 0.8) {
    //         speed = 1;
    //     }
        
    //     sleep_ms(1000);
    // }
}
