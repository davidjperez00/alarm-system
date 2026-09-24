#include "user_buttons.h"

#include <stddef.h>

// Driver wrapper libraries
#include "../gpio/gpio.h"

// espidf libraries
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// TODO: put these push buttons on an interrupt basis

#define USER_BUTTONS_ARM_IO_NUM 11          // GPIO pin number for arm pushutton
#define USER_BUTTONS_AUDIO_IO_NUM 10        // GPIO pin number for audio pushutton
#define USER_BUTTONS_NOTIFICATIONS_IO_NUM 9 // GPIO pin number for notifications pushutton
#define USER_BUTTONS_BATTERY_LIFE_IO_NUM 3  // GPIO pin number for battery life pushutton

#define USER_BUTTONS_INPUT_DEBOUNCE_MS 50

// New addition:
static TaskHandle_t input_monitor_task_handle;

static void input_monitor_task(void *arg)
{
    uint32_t triggered_pins;

    while (true)
    {
        xTaskNotifyWait(
            0,
            UINT32_MAX,
            &triggered_pins,
            portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(USER_BUTTONS_INPUT_DEBOUNCE_MS));

        if ((triggered_pins & (1UL << USER_BUTTONS_ARM_IO_NUM)) &&
            gpio_get_level(USER_BUTTONS_ARM_IO_NUM) == 0)
        {
            printf("Arm button pressed\r\n");
        }

        if ((triggered_pins & (1UL << USER_BUTTONS_AUDIO_IO_NUM)) &&
            gpio_get_level(USER_BUTTONS_AUDIO_IO_NUM) == 0)
        {
            printf("Audio I/O button pressed\r\n");
        }

        if ((triggered_pins & (1UL << USER_BUTTONS_NOTIFICATIONS_IO_NUM)) &&
            gpio_get_level(USER_BUTTONS_NOTIFICATIONS_IO_NUM) == 0)
        {
            printf("Notifications button pressed\r\n");
        }

        if ((triggered_pins & (1UL << USER_BUTTONS_BATTERY_LIFE_IO_NUM)) &&
            gpio_get_level(USER_BUTTONS_BATTERY_LIFE_IO_NUM) == 0)
        {
            printf("Battery Life button pressed\r\n");
        }
    }
}

static void IRAM_ATTR input_gpio_isr(void *arg)
{
    uint32_t gpio_num = (uint32_t)(uintptr_t)arg;
    BaseType_t task_woken = pdFALSE;

    xTaskNotifyFromISR(
        input_monitor_task_handle,
        1UL << gpio_num,
        eSetBits,
        &task_woken);

    portYIELD_FROM_ISR(task_woken);
}

// TODO: NOTE ONLY ARM BUTTON IS GOING TO BE CONFIGURED FIRST
/**
 * @brief Initiailizes push button gpios
 *
 * @warning At the time of writing this creates a task for debouncing
 *    gpios using an isr to notify the task. Additionally this function enables
 *    interrupts for the whole system; This should likely be done elsewhere.
 */
bool user_buttons_init(void)
{
    // TODO: Somehow this needs to make sure that the LEDs will be pushed on accordingly.

    // Ensure the gpio subsystem is initialized (registered with driver)
    if (!gpio_is_initialized())
    {
        printf("WARNING: %s failed, gpio subsystem isn't initialized\r\n", __func__);

        return false;
    }

    const uint32_t input_pins[] = {
        USER_BUTTONS_ARM_IO_NUM,
        USER_BUTTONS_AUDIO_IO_NUM,
        USER_BUTTONS_NOTIFICATIONS_IO_NUM,
        USER_BUTTONS_BATTERY_LIFE_IO_NUM,
    };

    // TODO: Check the gpio numbers aren't restricted

    // Create the task before enabling interrupts.
    // Otherwise an interrupt could occur while the task handle is NULL.
    BaseType_t task_result = xTaskCreate(
        input_monitor_task,
        "button_monitor",
        3072,
        NULL,
        5,
        &input_monitor_task_handle);

    if (task_result != pdPASS)
    {
        printf(
            "ERROR: %s failed to create button monitor task\r\n",
            __func__);

        return false;
    }

    // Initialize the io pin
    // TODO: This will require a reconfigure my code, I have esp32_gpio.c doing the masking
    // and my code only expected a uin32_t instead of the drivers uint64_t mask.
    custom_gpio_config_t base_io_config = {
        .gpio_num = 0,
        .gpio_mode = CUSTOM_GPIO_MODE_INPUT,
        .gpio_pull_up_en = CUSTOM_GPIO_PULLUP_DISABLE,
        .gpio_pull_down_en = CUSTOM_GPIO_PULLDOWN_DISABLE,
        .gpio_int_type = CUSTOM_GPIO_INTR_NEGEDGE,
    };

    for (size_t i = 0;
         i < sizeof(input_pins) / sizeof(input_pins[0]);
         i++)
    {
        base_io_config.gpio_num = input_pins[i];

        if (!gpio_configure_pin(base_io_config))
        {
            printf("ERROR: %s failed to initialize gpio_num = %ld failed to config\r\n", __func__, input_pins[i]);

            return false;
        }
    }

    /*
    NOTE: THE FOLLOWING CODE USES ESPIDF CALLS AND I'M NOT SURE I
            WANT THIS TO HAPPEN HERE.
    */

    // Enable interrupts on a per gpio basis; specified with no flags.
    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK)
    {
        printf("ERROR: %s failed, gpio_install_isr_service failed: %s\r\n", __func__, esp_err_to_name(err));

        return false;
    }

    for (size_t i = 0;
         i < sizeof(input_pins) / sizeof(input_pins[0]);
         i++)
    {
        err = gpio_isr_handler_add(
            input_pins[i],
            input_gpio_isr,
            (void *)(uintptr_t)input_pins[i]);

        if (err != ESP_OK)
        {
            printf(
                "ERROR: ISR setup failed for GPIO %ld: %s\r\n",
                input_pins[i],
                esp_err_to_name(err));

            return false;
        }
    }

    return true;
}
