#include "audio_amp.h"

#include "../../subsystems/gpio/gpio.h"

#include <stdio.h>

#define AUDIO_AMP_NOT_SD_BOOT_IO_NUM 12
#define AUDIO_AMP_GAIN_BIAS_IO_NUM 13
#define AUDIO_AMP_GAIN_DRV_IO_NUM 14

// Note this be default sets the gain to 3DB
bool audio_amp_init(void)
{

    // TODO: Consider configuring I2S lines here???

    // configure !SD_MODE pin to be (left + right)/2, meaning the amp expects stereo samples
    // and will sum then half them to be played on a single speaker.
    custom_gpio_config_t boot_io_config = {
        .gpio_num = AUDIO_AMP_NOT_SD_BOOT_IO_NUM,
        .gpio_mode = CUSTOM_GPIO_MODE_OUTPUT,
        .gpio_pull_up_en = CUSTOM_GPIO_PULLUP_DISABLE,
        .gpio_pull_down_en = CUSTOM_GPIO_PULLDOWN_DISABLE,
        .gpio_int_type = CUSTOM_GPIO_INTR_DISABLE,
    };

    if (!gpio_configure_pin(boot_io_config))
    {
        printf("ERROR: %s failed to initialize boot io config \r\n", __func__);

        return false;
    }

    gpio_write_pin(AUDIO_AMP_NOT_SD_BOOT_IO_NUM, 1);

    // NOTE: gain bias and gain drv IO pins work together below to set the default
    // gain to 3dB.
    if (!audio_amp_set_gain(AUDIO_AMP_GAIN_3DB))
    {
        printf("ERROR: %s failed to set audio amp gain \r\n", __func__);

        return false;
    }

    return true;
}

// TODO: Only 3dB works reliabily, due to insufficient regulator power
//     /*
//      * +-------------+------------------------+-----------------+-----------------+
//      * | Target gain | Datasheet wants        | GPIO_DRV        | GPIO_BIAS       |
//      * +-------------+------------------------+-----------------+-----------------+
//      * |     15 dB   | 100k pull-down to GND  | INPUT (Hi-Z)    | OUTPUT LOW      |
//      * |     12 dB   | GND                    | OUTPUT LOW      | INPUT (Hi-Z)    |
//      * |      9 dB   | Floating               | INPUT (Hi-Z)    | INPUT (Hi-Z)    |
//      * |      6 dB   | VIN                    | OUTPUT HIGH     | INPUT (Hi-Z)    |
//      * |      3 dB   | 100k pull-up to VIN    | INPUT (Hi-Z)    | OUTPUT HIGH     |
//      * +-------------+------------------------+-----------------+-----------------+
//      */
bool audio_amp_set_gain(audio_amp_gain gain)
{
    if (gain != AUDIO_AMP_GAIN_3DB)
    {
        printf("ERROR: %s only 3db gain is implemented at this moment. \r\n", __func__);
    }

    // configure gain bias gpio pin
    custom_gpio_config_t gain_bias_config = {
        .gpio_num = AUDIO_AMP_GAIN_BIAS_IO_NUM,
        .gpio_mode = CUSTOM_GPIO_MODE_OUTPUT,
        .gpio_pull_up_en = CUSTOM_GPIO_PULLUP_DISABLE,
        .gpio_pull_down_en = CUSTOM_GPIO_PULLDOWN_DISABLE,
        .gpio_int_type = CUSTOM_GPIO_INTR_DISABLE,
    };
    // configure gain drv gpio pin
    custom_gpio_config_t gain_drv_config = {
        .gpio_num = AUDIO_AMP_GAIN_DRV_IO_NUM,
        .gpio_mode = CUSTOM_GPIO_MODE_INPUT,
        .gpio_pull_up_en = CUSTOM_GPIO_PULLUP_DISABLE,
        .gpio_pull_down_en = CUSTOM_GPIO_PULLDOWN_DISABLE,
        .gpio_int_type = CUSTOM_GPIO_INTR_DISABLE,
    };

    if (!gpio_configure_pin(gain_bias_config))
    {
        printf("ERROR: %s failed to initialize gain bias config \r\n", __func__);
        return false;
    }
    if (!gpio_configure_pin(gain_drv_config))
    {
        printf("ERROR: %s failed to initialize gain drv config \r\n", __func__);
        return false;
    }

    gpio_write_pin(AUDIO_AMP_GAIN_BIAS_IO_NUM, 1);

    return true;
}

// AI generated code, perhaps this caused an issue with previous MAX98537 chip becoming non-functional.
// bool audio_amp_set_gain(audio_amp_gain gain)
// {
//     custom_gpio_mode_t bias_mode = CUSTOM_GPIO_MODE_INPUT;
//     custom_gpio_mode_t drv_mode = CUSTOM_GPIO_MODE_INPUT;

//     uint8_t bias_level = 0;
//     uint8_t drv_level = 0;

//     bool write_bias = false;
//     bool write_drv = false;

//     /*
//      * +-------------+------------------------+-----------------+-----------------+
//      * | Target gain | Datasheet wants        | GPIO_DRV        | GPIO_BIAS       |
//      * +-------------+------------------------+-----------------+-----------------+
//      * |     15 dB   | 100k pull-down to GND  | INPUT (Hi-Z)    | OUTPUT LOW      |
//      * |     12 dB   | GND                    | OUTPUT LOW      | INPUT (Hi-Z)    |
//      * |      9 dB   | Floating               | INPUT (Hi-Z)    | INPUT (Hi-Z)    |
//      * |      6 dB   | VIN                    | OUTPUT HIGH     | INPUT (Hi-Z)    |
//      * |      3 dB   | 100k pull-up to VIN    | INPUT (Hi-Z)    | OUTPUT HIGH     |
//      * +-------------+------------------------+-----------------+-----------------+
//      */

//     switch (gain)
//     {
//     case AUDIO_AMP_GAIN_15DB:
//         drv_mode = CUSTOM_GPIO_MODE_INPUT;
//         bias_mode = CUSTOM_GPIO_MODE_OUTPUT;
//         bias_level = 0;
//         write_bias = true;
//         break;

//     case AUDIO_AMP_GAIN_12DB:
//         drv_mode = CUSTOM_GPIO_MODE_OUTPUT;
//         bias_mode = CUSTOM_GPIO_MODE_INPUT;
//         drv_level = 0;
//         write_drv = true;
//         break;

//     case AUDIO_AMP_GAIN_9DB:
//         drv_mode = CUSTOM_GPIO_MODE_INPUT;
//         bias_mode = CUSTOM_GPIO_MODE_INPUT;
//         break;

//     case AUDIO_AMP_GAIN_6DB:
//         drv_mode = CUSTOM_GPIO_MODE_OUTPUT;
//         bias_mode = CUSTOM_GPIO_MODE_INPUT;
//         drv_level = 1;
//         write_drv = true;
//         break;

//     case AUDIO_AMP_GAIN_3DB:
//         drv_mode = CUSTOM_GPIO_MODE_INPUT;
//         bias_mode = CUSTOM_GPIO_MODE_OUTPUT;
//         bias_level = 1;
//         write_bias = true;
//         break;

//     default:
//         printf(
//             "ERROR: %s invalid audio amplifier gain: %d\r\n",
//             __func__,
//             (int)gain);

//         return false;
//     }

//     custom_gpio_config_t gain_bias_config = {
//         .gpio_num = AUDIO_AMP_GAIN_BIAS_IO_NUM,
//         .gpio_mode = bias_mode,
//         .gpio_pull_up_en = CUSTOM_GPIO_PULLUP_DISABLE,
//         .gpio_pull_down_en = CUSTOM_GPIO_PULLDOWN_DISABLE,
//         .gpio_int_type = CUSTOM_GPIO_INTR_DISABLE,
//     };

//     custom_gpio_config_t gain_drv_config = {
//         .gpio_num = AUDIO_AMP_GAIN_DRV_IO_NUM,
//         .gpio_mode = drv_mode,
//         .gpio_pull_up_en = CUSTOM_GPIO_PULLUP_DISABLE,
//         .gpio_pull_down_en = CUSTOM_GPIO_PULLDOWN_DISABLE,
//         .gpio_int_type = CUSTOM_GPIO_INTR_DISABLE,
//     };

//     if (!gpio_configure_pin(gain_bias_config))
//     {
//         printf(
//             "ERROR: %s failed to configure gain bias pin\r\n",
//             __func__);

//         return false;
//     }

//     if (!gpio_configure_pin(gain_drv_config))
//     {
//         printf(
//             "ERROR: %s failed to configure gain drive pin\r\n",
//             __func__);

//         return false;
//     }

//     if (write_bias)
//     {
//         gpio_write_pin(AUDIO_AMP_GAIN_BIAS_IO_NUM, bias_level);
//     }

//     if (write_drv)
//     {
//         gpio_write_pin(AUDIO_AMP_GAIN_DRV_IO_NUM, drv_level);
//     }

//     return true;
// }