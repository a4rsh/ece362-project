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


void draw_bg() {
    fillRect(0, 0, WIDTH, HORIZON, BLUE);
    fillRect(0, HEIGHT - HORIZON - 1, WIDTH, HEIGHT - HORIZON, GREEN);
}