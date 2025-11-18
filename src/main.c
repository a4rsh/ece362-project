#include "stdio.h"
#include "pico/stdlib.h"
#include "gyro.h"
#include "audio.h"
#include "game_dot.h"
#include <stdlib.h>

//Parameters
#define WIDTH 640
#define HEIGHT 480
#define NUM_SEGMENTS 40
#define POST_SPACING 8
#define ROAD_MIN_WIDTH 40
#define ROAD_MAX_WIDTH 200
#define GEAR_SPEEDS \
    X(0) \
    X(2) \
    X(4) \
    X(6) \
    X(9)
#define ACCEL 0.15
#define FRICTION 0.05
#define TURN_RATE 0.05
#define CENTER_SCREEN (WIDTH/2)
#define HORIZON (HEIGHT/2)

void vga_init();
void update_graphics(float angleX, float angleY, float angleZ, GameDot *dot1);

void test_gyro(void);
void test_audio(void);

void graphics_demo();
void start_game();


int main()
{
    stdio_init_all();
    sleep_ms(2000);

    // test_gyro();
    // test_audio();
    // test_vga();
    graphics_demo();

    graphics_game();

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

    GameDot dot1; 
    dot1.angleZ = 0.0;

    int res;
    float angleX = 0, angleY = 0, angleZ = 0;

    //This is low key unneccesary but I wanted to use X macros because I read about them
    #define X(val) val,
    const int gear_speeds[] = {
        GEAR_SPEEDS
    };
    #undef Xs

    printf("Speed gets hard set to gear speed 1 = %f\n", gear_speeds[0]);

    int speed = gear_speeds[0];
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

    srand(*SYST_CVR);

    for (;;)
    {
        res = updateAngles(&angleX, &angleY, &angleZ);
        if (res < 0)
        {
            printf("Read failed, %d\n", res);
            continue;
        }

        //Steering
        steer = angleY/180;
        player_x += steer * TURN_RATE * speed;

        //Random curvature
        #define MINWAIT 50
        #define MAXWAIT 400

        #define MINCURVE 10
        #define MAXCURVE 90
        
        curve_timer--;
        if (curve_timer <= 0) {
            curve_timer = MINWAIT + (rand() % (MAXWAIT - MINWAIT + 1));
            curve_target = MINCURVE + (rand() % (MAXCURVE - MINCURVE + 1));
        }
        road_curve += (curve_target - road_curve)/2;

        //Update road buffer
        divider_offset += speed;
        divider_offset %= 10;
        delta = roadBuffer[1] - roadBuffer[0];

        for(int i = 0; i < NUM_SEGMENTS - speed; i++) {
            roadBuffer[i] = roadBuffer[i + speed] - delta;
        }

        roadBuffer[0] = 0; //For our own sanity
        player_x -= delta * 3;
        
        roadBuffer[NUM_SEGMENTS - 1] = roadBuffer[NUM_SEGMENTS - 2] + road_curve;

        colorFlag ^= 1; //Update color in each row

        
        sleep_ms(5);
    }
}
