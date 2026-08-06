#ifndef SDCARD_DEVICE_H
#define SDCARD_DEVICE_H

#include <stdint.h>

// Indicates sdcard name and main directory
#define SD_CARD_MOUNT_POINT "/AUDIOSDCARD"
#define SD_CARD_MAIN_DIRECTORY "/"

void driver_esp32_sdcard_init(uint8_t gpio_num);

#endif // SDCARD_DEVICE_H
