#include "gpio.h"

#include <stdio.h>
#include <stdbool.h>

static const gpio_ops_t *gpio_backend = 0;
static bool is_initialized = false;

bool gpio_register_ops(const gpio_ops_t *ops)
{
    if (!ops)
    {
        printf("ERROR: %s Null pointer ops\r\n", __func__);

        return false;
    }

    gpio_backend = ops;

    is_initialized = true;

    return true;
}

bool gpio_is_initialized(void)
{
    return is_initialized;
}

bool gpio_configure_pin(custom_gpio_config_t gpio_config)
{

    if (!gpio_backend)
    {
        printf("ERROR: %s Null pointer gpio_backend \r\n", __func__);

        return false;
    }

    if (!gpio_backend->gpio_configure_pin(gpio_config))
    {
        printf("ERROR: %s gpio_backend->gpio_configure_pin() call failed \r\n", __func__);

        return false;
    }

    return true;
}

int32_t gpio_read_pin(uint32_t gpio_num)
{

    if (!gpio_backend)
    {
        printf("ERROR: %s Null pointer gpio_backend \r\n", __func__);

        return -1;
    }

    int32_t level = gpio_backend->gpio_read_pin(gpio_num);

    if (level == -1)
    {
        printf("ERROR: %s gpio_read_pin() call failed \r\n", __func__);

        return -1;
    }

    return level;
}

int32_t gpio_write_pin(uint32_t gpio_num, uint32_t level)
{

    if (!gpio_backend)
    {
        printf("ERROR: %s Null pointer gpio_backend \r\n", __func__);

        return false;
    }

    int32_t return_val = gpio_backend->gpio_write_pin(gpio_num, level);

    if (return_val == -1)
    {
        printf("ERROR: %s gpio_write_pin() call failed \r\n", __func__);

        return false;
    }

    return return_val;
}
