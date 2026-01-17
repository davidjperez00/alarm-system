#include "sdcard_device.h"

// esp-idf libraries
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

static sdmmc_card_t *card;

void driver_esp32_sdcard_init()
{
  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  host.max_freq_khz = 10000;

  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs = 5; // TODO: update to GPIO_NUM_5
  slot_config.host_id = SPI2_HOST;

  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 10,
  };

  ESP_ERROR_CHECK(esp_vfs_fat_sdspi_mount("/AUDIOSDCARD", &host, &slot_config, &mount_config, &card));
}