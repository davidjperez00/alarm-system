#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct i2s_ops
{
  bool (*init)(uint32_t sample_rate, uint8_t bits_per_sample, uint8_t channels);
  void (*deinit)();
  void (*write)(int16_t *data_buf, size_t buf_len);
} i2s_ops_t;

void i2s_register_ops(const i2s_ops_t *ops);
bool i2s_init(uint32_t sample_rate, uint8_t bits_per_sample, uint8_t channels);
void i2s_deinit();
// TODO: make this more flexible instead of just int16_t
void i2s_write(int16_t *data_buf, size_t buf_len);
