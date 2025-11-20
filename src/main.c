#include "stdio.h"
#include "pico/stdlib.h"
#include "gyro.h"
#include "audio.h"
#include "accel.h"
#include "game_dot.h"
#include <stdlib.h>
#include "game.h"
#include "hardware/irq.h"
#include "hardware/structs/systick.h"

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

    // int roadBuffer[HEIGHT-HORIZON];
    // int player_x = WIDTH / 2;

    // for(int i = 0; i < HEIGHT - HORIZON; i++) {
    //     roadBuffer[i] = WIDTH / 2 + i/5;
    //     printf("%d: %d\n", i, roadBuffer[i]);
    // }
    
    // draw_bg();
    // draw_road(roadBuffer, &player_x, 1);

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
    accel_init();

    int res;
    float angleX = 0, angleY = 0, angleZ = 0;

    //This is low key unneccesary but I wanted to use X macros because I read about them
    #define X(val) val,
    const int gear_speeds[] = {
        GEAR_SPEEDS
    };
    #undef X

    printf("Speed gets hard set to gear speed 1 = %d\n", gear_speeds[1]);

    int speed = gear_speeds[1];
    double steer;
    int curve_timer = 5;
    double curve_target;
    double road_curve = 0;
    int player_x = WIDTH/2;
    int delta;
    int colorFlag = 0;

    int roadBuffer[HEIGHT-HORIZON];

    //Initialize roadbuffer
    for(int i = 0; i < HEIGHT - HORIZON; i++) {
        roadBuffer[i] = WIDTH / 2;
    }

    srand((int) angleX * 100); //This is most likely wrong. And it's the reason everything breaks.

    int screen = 0; //This keeps track of what screen we are drawing to

    systick_hw -> rvr = 0xFFFFFF;
    systick_hw -> csr = 1 | (1 << 2);
    printf("csr: %d\n", systick_hw -> csr);

    printf("rvr: %d\n", systick_hw -> rvr);

    draw_bg(0);
    draw_bg(1);

    for (;;)
    {
        if(screen) {
            screen = 0;
        } else {
            screen = 1;
        }

        res = updateAngles(&angleX, &angleY, &angleZ);
        if (res < 0)
        {
            printf("Read failed, %d\n", res);
            continue;
        }

        //Steering
        steer = -angleZ;
        player_x += (int) steer * TURN_RATE * speed;

        //Random curvature
        #define MINWAIT 5
        #define MAXWAIT 10

        #define MINCURVE -0.5
        #define MAXCURVE 0.5
        curve_timer--;
        if (curve_timer <= 0) {
            curve_timer = MINWAIT + (rand() % (MAXWAIT - MINWAIT + 1));
            curve_target = MINCURVE + (double) rand()/RAND_MAX * (MAXCURVE - MINCURVE);
        }
        road_curve += (curve_target - road_curve)/2;

        //Update road buffer
        //divider_offset += speed;
        //divider_offset %= 10;
        delta = roadBuffer[1] - roadBuffer[0];

        for(int i = 0; i < HEIGHT - HORIZON - speed; i++) {
            roadBuffer[i] = roadBuffer[i + speed] - delta;
        }

        roadBuffer[0] = WIDTH/2; //For our own sanity
        player_x -= delta * 3;
    
        for(int i = 0; i < speed; i++) {
            roadBuffer[HEIGHT - HORIZON - speed + i] = roadBuffer[HEIGHT - HORIZON - speed + i - 1] + road_curve * i;
        }

        colorFlag ^= (speed % 2); //Update color in each row

        if(abs(player_x - roadBuffer[0]) > 30) {
//            speed = 1;
        }

        // volatile uint32_t before = systick_hw -> cvr;
        // volatile uint32_t flagBefore = systick_hw -> csr;
        // This lags a lot
        volatile uint64_t before = time_us_32();
        draw_road(roadBuffer, &player_x, colorFlag, screen);
        switchScreens(screen);
        volatile uint64_t after = time_us_32();
        // volatile uint32_t after = systick_hw -> cvr;
        // volatile uint32_t flagAfter = systick_hw -> csr >> 16;

        uint32_t difference = after - before;
        
        //printf("roadbuffer[0]: %d, player_x: %d, delta: %d, steer: %lf, curve_timer: %d, road_curve: %lf, speed: %d, before: %lu%lu, after: %lu%lu, difference: %u\n", roadBuffer[0], player_x, delta, steer, curve_timer, road_curve, speed, (uint32_t)(before >> 32), (uint32_t)(before & 0xffffffff), (uint32_t)(after >> 32), (uint32_t)(after & 0xffffffff), difference);
        //printf("difference: %u\n", difference);
        
        speed = (int) (updateSpeed() * 5);

        sleep_ms(30);
    }
}
