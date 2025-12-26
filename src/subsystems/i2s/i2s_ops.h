#include <stddef.h>

typedef struct i2s_ops
{
    void (*init)(int sample_rate, int bits_per_sample, int channels);
    void (*write)(int16_t *data_buf, size_t buf_len);
} i2s_ops_t;
