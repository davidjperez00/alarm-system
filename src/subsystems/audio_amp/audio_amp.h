#ifndef AUDIO_AMP_H
#define AUDIO_AMP_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    AUDIO_AMP_GAIN_3DB,
    AUDIO_AMP_GAIN_6DB,
    AUDIO_AMP_GAIN_9DB,
    AUDIO_AMP_GAIN_12DB,
    AUDIO_AMP_GAIN_15DB,
    AUDIO_AMP_GAIN_MAX,
} audio_amp_gain;

bool audio_amp_init(void);
bool audio_amp_set_gain(audio_amp_gain gain);

#endif // AUDIO_AMP_H
