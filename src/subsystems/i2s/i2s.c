#include "i2s.h"

static const i2s_ops_t *i2s_backend = 0;

void i2s_register_ops(const i2s_ops_t *ops) {
    i2s_backend = ops;
}

void i2s_init(int sample_rate, int bits_per_sample, int channels) {
    if (i2s_backend && i2s_backend->init)
        i2s_backend->init(sample_rate, bits_per_sample, channels);
}
