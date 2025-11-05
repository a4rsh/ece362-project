#ifndef AUDIO_H
#define AUDIO_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "ff.h"
#include "diskio.h"
#include <stdio.h>
#include <string.h>

#define AUDIO_BUFFER_SIZE 2048

void audio_init(void);
bool audio_play(const char *filename, uint8_t volume, bool loop);
void audio_stop(void);
void audio_set_volume(uint8_t volume);
void audio_update(void);

#endif
