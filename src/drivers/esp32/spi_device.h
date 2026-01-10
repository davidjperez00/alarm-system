#include <inttypes.h>
#include <stddef.h>

void driver_esp32_spi_register_ops(void);
void spi_write_esp32(int16_t *data, size_t len);
