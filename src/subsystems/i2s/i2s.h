#include "stdint.h"
#include "i2s_ops.h"

void i2s_register_ops(const i2s_ops_t *ops);
void i2s_init(int sample_rate, int bits_per_sample, int channels);
// TODO: make this more flexible instead of just int16_t
void i2s_write(int16_t *data_buf, size_t buf_len);
