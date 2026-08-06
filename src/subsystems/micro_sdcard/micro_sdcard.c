#include "micro_sdcard.h"

// Standard libraries
#include <stdio.h>

// My esp32 driver libraries
#include "../../drivers/esp32/sdcard_device.h"

// Driver wrapper libraries
#include "../spi/spi.h"
#include "../gpio/gpio.h"

// NOTE: The sd card uses the spi interface, see drivers/esp32/spi_device.c
// for the spi peripheral gpio pin numbers
#define MICRO_SDCARD_CD_IO_NUM 4 // housing physical card detect switch
#define MICRO_SDCARD_WP_IO_NUM 5 // housing physical card detect side spring
#define MICRO_SDCARD_CS_IO_NUM 6 // SPI chip select pin

bool micro_sdcard_init(void)
{
    // verify the spi databus is operating
    if (!spi_is_initialized())
    {
        printf("WARNING: %s failed, spi data bus not initialized\r\n", __func__);
        return false;
    }

    // TODO: ensure gpio system is registered???
    if (!gpio_is_initialized())
    {
        printf("WARNING: %s failed, gpio subsystem isn't initialized\r\n", __func__);

        return false;
    }

    // TODO: Check the gpio numbers aren't restricted?

    // initialize the gpio pins
    custom_gpio_config_t cd_io_config = {
        .gpio_num = MICRO_SDCARD_CD_IO_NUM,
        .gpio_mode = CUSTOM_GPIO_MODE_INPUT,
        .gpio_pull_up_en = CUSTOM_GPIO_PULLUP_DISABLE,
        .gpio_pull_down_en = CUSTOM_GPIO_PULLDOWN_DISABLE,
        .gpio_int_type = CUSTOM_GPIO_INTR_DISABLE,
    };

    custom_gpio_config_t wp_io_config = {
        .gpio_num = MICRO_SDCARD_WP_IO_NUM,
        .gpio_mode = CUSTOM_GPIO_MODE_INPUT,
        .gpio_pull_up_en = CUSTOM_GPIO_PULLUP_DISABLE,
        .gpio_pull_down_en = CUSTOM_GPIO_PULLDOWN_DISABLE,
        .gpio_int_type = CUSTOM_GPIO_INTR_DISABLE,
    };

    if (!gpio_configure_pin(cd_io_config))
    {
        printf("ERROR: %s failed to initialize cd io config \r\n", __func__);

        return false;
    }

    if (!gpio_configure_pin(wp_io_config))
    {
        printf("ERROR: %s failed to initialize wp io config \r\n", __func__);

        return false;
    }

    // Check if an sd card is inserted before trying to mount:
    if (!micro_sdcard_check_inserted())
    {
        printf("WARNING: %s failed, no micro sdcard inserted\r\n", __func__);

        return false;
    }

    // Initialize sd card driver and try to mount the file system
    driver_esp32_sdcard_init(MICRO_SDCARD_CS_IO_NUM);

    return true;
}

/**
 * @brief Checks if a micro sd card is inserted. This is true only if both
    pins are connected to ground.
 */
bool micro_sdcard_check_inserted(void)
{
    int32_t cd_pin_level = gpio_read_pin(MICRO_SDCARD_CD_IO_NUM);
    int32_t wp_pin_level = gpio_read_pin(MICRO_SDCARD_WP_IO_NUM);

    if (cd_pin_level == 0 &&
        wp_pin_level == 0)
    {
        return true;
    }

    printf("INFO: %s cd_pin_level = %ld \r\n", __func__, cd_pin_level);
    printf("INFO: %s wp_pin_level = %ld \r\n", __func__, wp_pin_level);

    return false;
}

// TODO: create a function to check if sd card is inserted?

// TODO: Is this useful to have?
//  bool micro_sdcard_self_test();