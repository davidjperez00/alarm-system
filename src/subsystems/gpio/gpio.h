#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>
#include <stdint.h>

// Generic wrapper for all gpio inputs,
// This is done to make our gpio function calls
// independent across different platforms, so only drivers
// need updated and not application code.
typedef enum
{
    CUSTOM_GPIO_MODE_DISABLE,         /*!< GPIO mode : disable input and output             */
    CUSTOM_GPIO_MODE_INPUT,           /*!< GPIO mode : input only                           */
    CUSTOM_GPIO_MODE_OUTPUT,          /*!< GPIO mode : output only mode                     */
    CUSTOM_GPIO_MODE_OUTPUT_OD,       /*!< GPIO mode : output only with open-drain mode     */
    CUSTOM_GPIO_MODE_INPUT_OUTPUT_OD, /*!< GPIO mode : output and input with open-drain mode*/
    CUSTOM_GPIO_MODE_INPUT_OUTPUT,    /*!< GPIO mode : output and input mode                */
} custom_gpio_mode_t;

typedef enum
{
    CUSTOM_GPIO_PULLUP_DISABLE = 0x0, /*!< Disable GPIO pull-up resistor */
    CUSTOM_GPIO_PULLUP_ENABLE = 0x1,  /*!< Enable GPIO pull-up resistor */
} custom_gpio_pullup_t;

typedef enum
{
    CUSTOM_GPIO_PULLDOWN_DISABLE = 0x0, /*!< Disable GPIO pull-down resistor */
    CUSTOM_GPIO_PULLDOWN_ENABLE = 0x1,  /*!< Enable GPIO pull-down resistor  */
} custom_gpio_pulldown_t;

typedef enum
{
    CUSTOM_GPIO_INTR_DISABLE = 0,    /*!< Disable GPIO interrupt                             */
    CUSTOM_GPIO_INTR_POSEDGE = 1,    /*!< GPIO interrupt type : rising edge                  */
    CUSTOM_GPIO_INTR_NEGEDGE = 2,    /*!< GPIO interrupt type : falling edge                 */
    CUSTOM_GPIO_INTR_ANYEDGE = 3,    /*!< GPIO interrupt type : both rising and falling edge */
    CUSTOM_GPIO_INTR_LOW_LEVEL = 4,  /*!< GPIO interrupt type : input low level trigger      */
    CUSTOM_GPIO_INTR_HIGH_LEVEL = 5, /*!< GPIO interrupt type : input high level trigger     */
    CUSTOM_GPIO_INTR_MAX,
} custom_gpio_int_type_t;

typedef struct
{
    uint32_t gpio_num;
    custom_gpio_mode_t gpio_mode;
    custom_gpio_pullup_t gpio_pull_up_en;
    custom_gpio_pulldown_t gpio_pull_down_en;
    custom_gpio_int_type_t gpio_int_type;
} custom_gpio_config_t;

typedef struct gpio_ops_t
{
    bool (*gpio_configure_pin)(custom_gpio_config_t gpio_config);
    int32_t (*gpio_read_pin)(uint32_t gpio_num);
    int32_t (*gpio_write_pin)(uint32_t gpio_num, uint32_t level);
} gpio_ops_t;

// TODO: consider making these const of pointers
// do avoid function calls copying structure!
bool gpio_register_ops(const gpio_ops_t *ops);
bool gpio_is_initialized(void);
bool gpio_configure_pin(custom_gpio_config_t gpio_config);
int32_t gpio_read_pin(uint32_t gpio_num);
int32_t gpio_write_pin(uint32_t gpio_num, uint32_t level);

#endif // GPIO_H
