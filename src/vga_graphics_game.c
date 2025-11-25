// VGA graphics library
#include "vga_graphics.h"
#include <stdio.h>
#include <stdlib.h>
// #include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "game.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

void vga_init() {
    // Initialize the VGA screen
    initVGA();
}


void draw_bg(int crash, int screen) {

    char blue = BLUE;
    char black = BLACK;

    if(crash) {
        blue |= 0x9;
        black |= 0x9;
    }

    fillRectScreenSelect(0, 0, WIDTH, HORIZON, blue, screen);
    //fillRectScreenSelect(0, HEIGHT - HORIZON - 1, WIDTH, HEIGHT - HORIZON, GREEN, screen);
    drawCharScreenSelect(50, 50, 'S', black, blue, 5, screen);
    drawCharScreenSelect(80, 50, 'c', black, blue, 5, screen);
    drawCharScreenSelect(110, 50, 'o', black, blue, 5, screen);
    drawCharScreenSelect(140, 50, 'r', black, blue, 5, screen);
    drawCharScreenSelect(170, 50, 'e', black, blue, 5, screen);
    drawCharScreenSelect(195, 40, ':', black, blue, 8, screen);
}

void draw_score(int score, int crash, int screen) {
    int scoreToDisp;
    scoreToDisp = MIN(score, 99999);

    int firstDigit = scoreToDisp/10000;
    int secondDigit = (scoreToDisp % 10000) /1000;
    int thirdDigit = (scoreToDisp % 1000) /100;
    int fourthDigit = (scoreToDisp % 100) / 10;
    int fifthDigit = scoreToDisp % 10;

    char firstChar = firstDigit + 48;
    char secondChar = secondDigit + 48;
    char thirdChar = thirdDigit + 48;
    char fourthChar = fourthDigit + 48;
    char fifthChar = fifthDigit + 48;

    char black = BLACK;
    char blue = BLUE;

    if(crash) {
        black |= 0x9;
        blue |= 0x9;
    }

    drawCharScreenSelect(225, 50, firstChar, black, blue, 5, screen);
    drawCharScreenSelect(255, 50, secondChar, black, blue, 5, screen);
    drawCharScreenSelect(285, 50, thirdChar, black, blue, 5, screen);
    drawCharScreenSelect(315, 50, fourthChar, black, blue, 5, screen);
    drawCharScreenSelect(345, 50, fifthChar, black, blue, 5, screen);
}

void draw_road(int* roadBuffer, int* player_x, int colorFlag, int crash, int screen) {

    double depth;
    int width;
    int cx;
    int i;
    char color;

    for(int y = HEIGHT - 1; y >= HORIZON; y--) {
        i = (HEIGHT - 1) - y;

        depth = (float) (y - HORIZON) / (HEIGHT - HORIZON);
        width = ROAD_MIN_WIDTH + depth * (ROAD_MAX_WIDTH - ROAD_MIN_WIDTH);
        cx = CENTER_SCREEN + ((roadBuffer[i] - *player_x)) * (2*(1-depth) + 1);

        if ((i + colorFlag) % 2 == 0) {
            color = RED;
        } else {
            color = WHITE;
        }

        char black = BLACK;
        char green = GREEN;

        if(crash) {
            color = color | 0x9;
            black |= 0x9;
            green |= 0x9;
        }

        // printf("y: %d\n", y);
        // printf("i: %d\n", i);
        // printf("depth: %lf\n", depth);
        // printf("width: %d\n", width);
        // printf("cx: %d\n", cx);

            if((cx + width < WIDTH) && (cx - width) >= 0) {
                fillRectScreenSelect((cx - width), y, width * 2, 1, black, screen);
                fillRectScreenSelect(0, y, cx - width, 1, green, screen);
                fillRectScreenSelect(cx + width, y, WIDTH - (cx + width), 1, green, screen);

                if(y % 7 == 0 && (y + 7) < HEIGHT) {
                    fillRectScreenSelect((cx - width) - 7, y, 7, 7, color, screen);
                    fillRectScreenSelect((cx + width) - 3, y, 7, 7, color, screen);
                }
            } else if ((cx - width) < 0) {
                if (cx >= 0) {

                    fillRectScreenSelect(0, y, cx, 1, black, screen);
                    fillRectScreenSelect(cx, y, width, 1, black, screen);
                    fillRectScreenSelect(cx + width, y, WIDTH - (cx + width), 1, green, screen);
                    if(y % 7 == 0 && (y + 7) < HEIGHT) {
                        fillRectScreenSelect((cx + width) - 3, y, 7, 7, color, screen);
                    }
                } else if ((cx + width) > 0) {
                fillRectScreenSelect(0, y, width - cx, 1, black, screen);
                fillRectScreenSelect(cx + width, y, WIDTH - (cx + width), 1, green, screen);
                if(y % 7 == 0 && (y + 7) < HEIGHT) {
                    fillRectScreenSelect((cx + width) - 3, y, 7, 7, color, screen);
                }
                } else {
                    fillRectScreenSelect(0, y, WIDTH, 1, green, screen);
                }
            } else { //cx + width < WIDTH
                if (cx < WIDTH) {
                    fillRectScreenSelect(0, y, (cx - width), 1, green, screen);
                    fillRectScreenSelect(cx - width, y, width, 1, black, screen);
                    fillRectScreenSelect(cx, y, WIDTH - cx, 1, black, screen);
                    if(y % 7 == 0 && (y + 7) < HEIGHT) {
                        fillRectScreenSelect((cx - width) - 3, y, 7, 7, color, screen);
                    }
                } else if ((cx - width) < WIDTH) {
                    fillRectScreenSelect(0, y, cx - width, 1, green, screen);
                    fillRectScreenSelect(cx - width, y, WIDTH - (cx - width), 1, black, screen);
                    if(y % 7 == 0 && (y + 7) < HEIGHT) {
                        fillRectScreenSelect((cx - width) - 3, y, 7, 7, color, screen);
                    }
                } else {
                    fillRectScreenSelect(0, y, WIDTH, 1, green, screen);
                }
            }
    }
}