#include "i2s.h"

static const i2s_ops_t *i2s_backend = 0;

void i2s_register_ops(const i2s_ops_t *ops)
{
    i2s_backend = ops;
}

void i2s_init(uint32_t sample_rate, int bits_per_sample, int channels)
{
    if (i2s_backend && i2s_backend->init)
        i2s_backend->init(sample_rate, bits_per_sample, channels);
}

void i2s_write(int16_t *data, size_t data_len)
{
    if (i2s_backend && i2s_backend->write)
    {
        i2s_backend->write(data, data_len);
    }
}
