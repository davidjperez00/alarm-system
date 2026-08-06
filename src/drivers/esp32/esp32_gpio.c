#include "esp32_gpio.h"

#include "gpio.h" // Custom gpio wrapper

#include "driver/gpio.h" // for gpio_config_t, gpio_config()
#include "esp_err.h"     // for esp_err_t, esp_err_to_name()

#include <stdio.h>

// Static function definitions:
static bool esp32_gpio_configure_pin(custom_gpio_config_t cust_gpio_config);
static int32_t esp32_gpio_read_pin(uint32_t gpio_num);
static int32_t esp32_gpio_write_pin(uint32_t gpio_num, uint32_t level);

// Struct to match with our subsystem gpio wrapper,
// This is to allow this driver code to be hot-swappable
// for other drivers.
static const gpio_ops_t esp32_gpio_ops = {
    .gpio_configure_pin = esp32_gpio_configure_pin,
    .gpio_read_pin = esp32_gpio_read_pin,
    .gpio_write_pin = esp32_gpio_write_pin,
};

static const uint64_t restricted_gpio_pins[] = {
    GPIO_NUM_0,  // BOOT MODE to enter flash mode
    GPIO_NUM_35, // PSRAM reserved pin
    GPIO_NUM_36, // PSRAM reserved pin
    GPIO_NUM_37, // PSRAM reserved pin
    // GPIO_NUM_2,  // Strapped at boot, high for normal boot
    // GPIO_NUM_12, // Strapped at boot, controls VDD_SDIO
    // GPIO_NUM_15, // Pulled LOW for boot mode (sed with SDIO sometimes)
    // GPIO_NUM_6,  // Flash/SPI
    // GPIO_NUM_7,  // Flash/SPI
    // GPIO_NUM_8,  // Flash/SPI
    // GPIO_NUM_9,  // Flash/SPI
    // GPIO_NUM_10, // Flash/SPI
    // GPIO_NUM_11, // Flash/SPI
    // GPIO_NUM_1,  // TX for for serial coms, (used for flashing)
    // GPIO_NUM_3   // RX for for serial coms, (used for flashing)

};

// When this is called, subsystem/gpio will use this esp32 driver code.
bool esp32_gpio_driver_register_ops(void)
{
    if (!gpio_register_ops(&esp32_gpio_ops))
    {
        printf("ERROR: %s failed to register\r\n", __func__);

        return false;
    }

    return true;
}

// TODO: Make my own data type to cast into this?
static bool esp32_gpio_configure_pin(custom_gpio_config_t cust_gpio_config)
{
    // Check that the pin to be configured is not a restricted pin.
    for (int i = 0; i < sizeof(restricted_gpio_pins) / sizeof(restricted_gpio_pins[0]); i++)
    {
        if (cust_gpio_config.gpio_num == restricted_gpio_pins[i])
        {
            printf("ERROR: %s Invalid gpio_num, gpio_num = %lu\r\n", __func__, cust_gpio_config.gpio_num);

            return false;
        }
    }

    if (cust_gpio_config.gpio_num > GPIO_NUM_39)
    {
        printf("ERROR: %s gpio_num too large, gpio_num = %lu\r\n", __func__, cust_gpio_config.gpio_num);

        return false;
    }

    // TODO: There are other configurations available but I'm choosing to intentionally
    // ignore them for now as I have to make a specific type cast.
    if ((cust_gpio_config.gpio_mode != CUSTOM_GPIO_MODE_INPUT) &&
        (cust_gpio_config.gpio_mode != CUSTOM_GPIO_MODE_OUTPUT))
    {
        printf("ERROR: %s Invalid pin mode, gpio_pinmode= %u", __func__, cust_gpio_config.gpio_mode);

        return false;
    }

    // Configure GPIO as input with internal pull-up
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << cust_gpio_config.gpio_num),
        .mode = cust_gpio_config.gpio_mode,
        .pull_up_en = cust_gpio_config.gpio_pull_up_en,
        .pull_down_en = cust_gpio_config.gpio_pull_down_en,
        .intr_type = cust_gpio_config.gpio_int_type};

    if (gpio_config(&io_conf) != ESP_OK)
    {
        printf("ERROR: %s gpio_config() failed", __func__);
    }

    return true;
}

//@warning If the pad is not configured for input (or input and output) the returned value is always 0.
static int32_t esp32_gpio_read_pin(uint32_t gpio_num)
{
    // Check if we have a valid gpio number
    if (gpio_num > GPIO_NUM_39)
    {
        printf("ERROR: %s gpio_num too large, gpio_num = %lu\r\n", __func__, gpio_num);

        return -1;
    }

    return gpio_get_level(gpio_num);
}

static int32_t esp32_gpio_write_pin(uint32_t gpio_num, uint32_t level)
{
    if (gpio_num > GPIO_NUM_39)
    {
        printf("ERROR: %s gpio_num too large, gpio_num = %lu\r\n", __func__, gpio_num);

        return -1;
    }

    if ((level != 0) && (level != 1))
    {
        printf("ERROR: %s level invalid, level = %lu\r\n", __func__, gpio_num);

        return -1;
    }

    if (gpio_set_level(gpio_num, level))
    {
        printf("ERROR: %s gpio_set_level() call failed", __func__);

        return -1;
    }

    return 0;
}
