// VGA graphics library
#include "vga_graphics.h"
#include <stdio.h>
#include <stdlib.h>
// #include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "game.h"

void vga_init() {
    // Initialize the VGA screen
    initVGA();
}


void draw_bg(int screen) {
    fillRectScreenSelect(0, 0, WIDTH, HORIZON, BLUE, screen);
    //fillRectScreenSelect(0, HEIGHT - HORIZON - 1, WIDTH, HEIGHT - HORIZON, GREEN, screen);
}

void draw_road(int* roadBuffer, int* player_x, int colorFlag, int screen) {

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

        // printf("y: %d\n", y);
        // printf("i: %d\n", i);
        // printf("depth: %lf\n", depth);
        // printf("width: %d\n", width);
        // printf("cx: %d\n", cx);

        if((cx + width < WIDTH) && (cx - width) >= 0) {
            fillRectScreenSelect((cx - width), y, width * 2, 1, BLACK, screen);
            fillRectScreenSelect(0, y, cx - width, 1, GREEN, screen);
            fillRectScreenSelect(cx + width, y, WIDTH - (cx + width), 1, GREEN, screen);

            if(y % 7 == 0 && (y + 7) < HEIGHT) {
                fillRectScreenSelect((cx - width) - 7, y, 7, 7, color, screen);
                fillRectScreenSelect((cx + width) - 3, y, 7, 7, color, screen);
            }
        } else if ((cx - width) < 0) {
            if (cx >= 0) {

                fillRectScreenSelect(0, y, cx, 1, BLACK, screen);
                fillRectScreenSelect(cx, y, width, 1, BLACK, screen);
                fillRectScreenSelect(cx + width, y, WIDTH - (cx + width), 1, GREEN, screen);
                if(y % 7 == 0 && (y + 7) < HEIGHT) {
                    fillRectScreenSelect((cx + width) - 3, y, 7, 7, color, screen);
                }
            } else if ((cx + width) > 0) {
            fillRectScreenSelect(0, y, width - cx, 1, BLACK, screen);
            fillRectScreenSelect(cx + width, y, WIDTH - (cx + width), 1, GREEN, screen);
            if(y % 7 == 0 && (y + 7) < HEIGHT) {
                fillRectScreenSelect((cx + width) - 3, y, 7, 7, color, screen);
            }
            } else {
                fillRectScreenSelect(0, y, WIDTH, 1, GREEN, screen);
            }
        } else { //cx + width < WIDTH
            if (cx < WIDTH) {
                fillRectScreenSelect(0, y, (cx - width), 1, GREEN, screen);
                fillRectScreenSelect(cx - width, y, width, 1, BLACK, screen);
                fillRectScreenSelect(cx, y, WIDTH - cx, 1, BLACK, screen);
                if(y % 7 == 0 && (y + 7) < HEIGHT) {
                    fillRectScreenSelect((cx - width) - 3, y, 7, 7, color, screen);
                }
            } else if ((cx - width) < WIDTH) {
                fillRectScreenSelect(0, y, cx - width, 1, GREEN, screen);
                fillRectScreenSelect(cx - width, y, WIDTH - (cx - width), 1, BLACK, screen);
                if(y % 7 == 0 && (y + 7) < HEIGHT) {
                    fillRectScreenSelect((cx - width) - 3, y, 7, 7, color, screen);
                }
            } else {
                fillRectScreenSelect(0, y, WIDTH, 1, GREEN, screen);
            }
        }
    }
}