#include "i2s.h"

#include <stdio.h>

static const i2s_ops_t *i2s_backend = 0;

void i2s_register_ops(const i2s_ops_t *ops)
{
    if (!ops)
    {
        printf("ERROR: %s NULL pointer 'ops' argument\r\n", __func__);
        return;
    }

    i2s_backend = ops;
}

bool i2s_init(uint32_t sample_rate, uint8_t bits_per_sample, uint8_t channels)
{
    // Check if ops was registered and an init function for the op that was registered exists
    if (!i2s_backend || !i2s_backend->init)
    {
        printf("ERROR: %s No backend connected, not init function to run\r\n", __func__);
        return false;
    }

    if (!i2s_backend->init(sample_rate, bits_per_sample, channels))
    {
        printf("ERROR: %s Backend system init failed\r\n", __func__);
        return false;
    }

    return true;
}
void i2s_deinit()
{
    if (!i2s_backend || !i2s_backend->deinit)
    {
        printf("ERROR: %s No backend connected, not deinit function to run\r\n", __func__);
        return;
    }

    i2s_backend->deinit();
}

void i2s_write(int16_t *data, size_t data_len)
{
    if (!i2s_backend || !i2s_backend->write)
    {
        printf("ERROR: %s No backend connected, not write function to run\r\n", __func__);
        return;
    }

    i2s_backend->write(data, data_len);
}
