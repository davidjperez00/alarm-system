#pragma once

typedef struct i2s_ops {
    void (*init)(int sample_rate, int bits_per_sample, int channels);
} i2s_ops_t;

void i2s_register_ops(const i2s_ops_t *ops);
void i2s_init(int sample_rate, int bits_per_sample, int channels);
