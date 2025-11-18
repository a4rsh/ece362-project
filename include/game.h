void vga_init();
void draw_bg();

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


