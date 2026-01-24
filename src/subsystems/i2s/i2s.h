#include <stdint.h>
#include <stddef.h>

typedef struct i2s_ops
{
  void (*init)(uint32_t sample_rate, int bits_per_sample, int channels);
  void (*deinit)();
  void (*write)(int16_t *data_buf, size_t buf_len);
} i2s_ops_t;

void i2s_register_ops(const i2s_ops_t *ops);
void i2s_init(uint32_t sample_rate, int bits_per_sample, int channels);
void i2s_deinit();
// TODO: make this more flexible instead of just int16_t
void i2s_write(int16_t *data_buf, size_t buf_len);
