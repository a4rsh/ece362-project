#include "accel.h"
#include "math.h"

const int GPIO_ACCEL = 26;
const int GPIO_BRAKE = 25;

float speed;
float time;

void accel_init() {
    gpio_init(GPIO_ACCEL);
    gpio_init(GPIO_BRAKE);

    speed = 0;
    time = get_absolute_time();
}

float updateSpeed() {
    bool accel = gpio_get(GPIO_ACCEL);
    bool brake = gpio_get(GPIO_BRAKE);
        
    float change = -0.00000002;
    float newTime = get_absolute_time();

    if (accel ^ brake) {
        if (accel) {
            change += 0.000001; 
        } else {
            change -= 0.0000008;
        }
    }

    speed += change * (newTime - time);
    speed = fmax(fmin(speed, 1.0), 0.0);

    time = newTime;

    return speed;
}