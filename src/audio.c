#include "audio.h"

const int PWM_AUDIO_RIGHT = 6;
const int PWM_AUDIO_LEFT = 7;

const int PERIOD = 440;

void audio_init () {
    gpio_set_function(PWM_AUDIO_RIGHT, GPIO_FUNC_PWM);
    gpio_set_function(PWM_AUDIO_LEFT, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(PWM_AUDIO_RIGHT);

    pwm_set_wrap(slice_num, PERIOD + 1);
    
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 1);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 3);

    pwm_set_enabled(slice_num, true);
}