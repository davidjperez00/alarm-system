#include "spi_device.h"
#include "spi.h"

// esp-idf defines
#include "driver/spi_common.h"
#include "driver/gpio.h"

// ESP defined GPIO pins used for spi 1
// #define SPI_SC0 GPIO_NUM_15  // spi data in io number
#define SPI_SCLK GPIO_NUM_2  // spi bit clock io number
#define SPI_MISO GPIO_NUM_38 // spi word select io number
#define SPI_MOSI GPIO_NUM_1  // spi data out io number

// Static function definitions:
static void driver_esp32_spi_init();
static void driver_esp32_spi_write(int16_t *data_buf, size_t buf_len);

// Struct to match with our subsystem spi wrapper,
// This is to allow this driver code to be hot-swappable
// for other drivers.
static const spi_ops_t driver_esp32_spi_ops = {
    .init = driver_esp32_spi_init,
    .write = driver_esp32_spi_write,
};

// When this is called, subsystem/spi will use this esp32 driver code.
void driver_esp32_spi_register_ops(void)
{
  spi_register_ops(&driver_esp32_spi_ops);
}

// TODO: incorporate bits_per_sample and channels
static void driver_esp32_spi_init()
{
  spi_bus_config_t bus_config = {
      .mosi_io_num = SPI_MOSI,
      .miso_io_num = SPI_MISO,
      .sclk_io_num = SPI_SCLK,

      // Not used in SPI mode
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .data4_io_num = -1,
      .data5_io_num = -1,
      .data6_io_num = -1,
      .data7_io_num = -1,

      .max_transfer_sz = 4096, // Safe default for SD card blocks
      // TODO: what are the bus flags
      .flags = SPICOMMON_BUSFLAG_MASTER,
      .intr_flags = 0,
  };

  // TODO: investigate what diff dma channels there are
  ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));
}

static void driver_esp32_spi_write(int16_t *data_buf, size_t buf_len)
{
  if (data_buf == NULL)
  {
    printf("ERROR: data_buf NULL \r\n");
  }
}
