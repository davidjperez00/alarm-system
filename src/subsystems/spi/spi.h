#include <stdint.h>
#include <stddef.h>

typedef struct spi_ops
{
  void (*init)();
  void (*write)(int16_t *data, size_t data_len);
} spi_ops_t;

void spi_register_ops(const spi_ops_t *ops);
void spi_init();
// TODO: make this more flexible instead of just int16_t
void spi_write(int16_t *data_buf, size_t buf_len);
