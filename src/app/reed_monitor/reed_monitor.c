#include "reed_monitor.h"

#include "../../subsystems/gpio/gpio.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// TODO: This is to get time, replace with a wrapper
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define REED_SWITCH_GPIO_NUM 36

static bool reed_monitor_is_initialized = false;

/**  Start: Debounce Variables **/
static int32_t lastRawPinLevel = 0;
static int32_t lastStablePinLevel = 0;
// Used to track when pin value changed and to ensure
// we wait long enough before declaring pin value has
// settled during debounce.
static uint64_t debounceTimerMs = 0;
// number of us to debounce wait for
#define REED_MONITOR_DEBOUNCE_MS 50
/**  End: Debounce Variables **/

// TODO: register interrupts for the gpio pin
bool reed_monitor_init()
{
    if (reed_monitor_is_initialized)
    {
        printf("WARNING: %s system already initialized\r\n", __func__);

        return false;
    }

    // Configure the reed monitor gpio.
    custom_gpio_config_t reed_io_config = {
        .gpio_num = REED_SWITCH_GPIO_NUM,
        .gpio_mode = CUSTOM_GPIO_MODE_INPUT,
        .gpio_pull_up_en = CUSTOM_GPIO_PULLUP_DISABLE,
        .gpio_pull_down_en = CUSTOM_GPIO_PULLDOWN_DISABLE,
        .gpio_int_type = CUSTOM_GPIO_INTR_DISABLE,
    };

    if (!gpio_configure_pin(reed_io_config))
    {
        printf("ERROR: %s failed to initialize \r\n", __func__);

        return false;
    }

    // TODO: Probably want to read the pin value when
    // the system is started and the interrupt is setup

    reed_monitor_is_initialized = true;

    return true;
}

// Make static and have a handler for this:
bool reed_monitor_debounce_pin()
{
    // TODO: First check interrupt flag when interrupt is setup

    // Read the pin
    int32_t pin_level = gpio_read_pin(REED_SWITCH_GPIO_NUM);

    if (pin_level == -1)
    {
        printf("ERROR: %s gpio_read_pin() failed\r\n", __func__);

        return false;
    }

    if (pin_level != lastRawPinLevel)
    {
        lastRawPinLevel = pin_level;

        // Get time pin value changed, this us since boot
        // should last about 292k years.
        TickType_t now = xTaskGetTickCount();                 // ticks since boot
        debounceTimerMs = (uint64_t)now * portTICK_PERIOD_MS; // convert to ms
    }

    uint64_t now_ms = (uint64_t)xTaskGetTickCount() * portTICK_PERIOD_MS;

    if ((now_ms - debounceTimerMs) >= REED_MONITOR_DEBOUNCE_MS)
    {
        if (pin_level != lastStablePinLevel)
        {
            // Pin successfully debounced
            lastStablePinLevel = pin_level;
            printf("PIN DEBOUNCED: value = %ld\r\n", lastStablePinLevel);
        }
    }

    return true;
}