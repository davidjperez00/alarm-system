#include "spi.h"

static const spi_ops_t *spi_backend = 0;

void spi_register_ops(const spi_ops_t *ops)
{
  spi_backend = ops;
}

void spi_init()
{
  if (spi_backend && spi_backend->init)
    spi_backend->init();
}

void spi_write(int16_t *data, size_t data_len)
{
  if (spi_backend && spi_backend->write)
  {
    spi_backend->write(data, data_len);
  }
}
