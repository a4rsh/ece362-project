void vga_init();
void draw_bg(int screen);
void draw_road(int* roadBuffer, int* player_x, int colorFlag, int screen);

//Parameters
#define WIDTH 640
#define HEIGHT 480
#define POST_SPACING 8
#define ROAD_MIN_WIDTH 40
#define ROAD_MAX_WIDTH 200
#define GEAR_SPEEDS \
    X(0) \
    X(5) \
    X(10) \
    X(15) \
    X(20)
#define ACCEL 0.15
#define FRICTION 0.05
#define TURN_RATE 0.05
#define CENTER_SCREEN (WIDTH/2)
#define HORIZON (HEIGHT/2)


