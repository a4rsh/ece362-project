// TODO: Loop Audio with a timer so calling function doesn't have to loop
//       Setup timer in audio_init() with audio_update() hander 
//       Timer gets set sample_delay_us in the future from audio_play and end of audio_update()

// TODO: Fill buffer automatically using DMA


#include "audio.h"

static const uint PWM_AUDIO_RIGHT = 6;
static const uint PWM_AUDIO_LEFT = 7;
static const uint PERIOD = 1023;
static uint32_t sample_delay_us = 32;
// sample_delay_us = (1000000 / sample_rate) - 30;
// if (sample_delay_us < 5) sample_delay_us = 5;

static FIL audio_file;
static bool audio_active = false;
static bool audio_loop = false;
static uint32_t buffer_pos = 0;
static uint32_t buffer_len = 0;
static int16_t buffer[AUDIO_BUFFER_SIZE];
static uint8_t volume = 128;
static uint slice_num;


static void audio_set_pwm(int level) {
    pwm_set_chan_level(slice_num, PWM_CHAN_A, level);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, level);
}

// For refilling SD Buffer early
static void refill_buffer(void) {
    if (!audio_active) return;

    UINT br;
    f_read(&audio_file, buffer, sizeof(buffer), &br);
    buffer_len = br / 2;
    buffer_pos = 0;

    if (buffer_len == 0) {
        if (audio_loop) {
            f_lseek(&audio_file, 44);
            refill_buffer();
        } else {
            audio_active = false;
        }
    }
}

void audio_init(void) {
    if (!sd_init()) {
        printf("SD init failed\n");
        return;
    }

    gpio_set_function(PWM_AUDIO_RIGHT, GPIO_FUNC_PWM);
    gpio_set_function(PWM_AUDIO_LEFT, GPIO_FUNC_PWM);

    slice_num = pwm_gpio_to_slice_num(PWM_AUDIO_RIGHT);
    pwm_set_wrap(slice_num, PERIOD);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 512);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 512);
    pwm_set_enabled(slice_num, true);

    audio_active = false;
    buffer_pos = 0;
    buffer_len = 0;
}

bool audio_play(const char *filename, uint8_t vol, bool loop) {
    if (audio_active) f_close(&audio_file);

    if (f_open(&audio_file, filename, FA_READ) != FR_OK) {
        printf("Failed to open file: %s\n", filename);
        audio_active = false;
        return false;
    }

    buffer_pos = 0;
    buffer_len = 0;
    volume = vol;
    audio_loop = loop;
    
    audio_active = true;
    refill_buffer();
    return true;
}

void audio_stop(void) {
    if (audio_active) f_close(&audio_file);
    audio_active = false;
    audio_set_pwm(PERIOD / 2);
}

void audio_set_volume(uint8_t vol) {
    volume = vol;
}


void audio_update(void) {
    if (!audio_active) {
        audio_set_pwm(PERIOD / 2);
        sleep_us(sample_delay_us);
        return;
    }

    if (buffer_pos >= (buffer_len * 3 / 4)) {
        refill_buffer();
    }

    int16_t sample = buffer[buffer_pos++];
    
    int32_t scaled = (sample * (int32_t)volume) >> 8;
    
    scaled = (scaled >> 6) + (PERIOD / 2);
    
    if (scaled < 0) scaled = 0;
    if (scaled > PERIOD) scaled = PERIOD;

    audio_set_pwm(scaled);
    sleep_us(sample_delay_us);
}