#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
// #include "audio_i2s.h" // fuctom fo

#include "driver/i2s_std.h"
#include "esp_log.h"

// DEBUG: used for sanity checking program running
#include "driver/gpio.h"
#define LED_GPIO 2   // change if your board uses a different pin


#define I2S_SAMPLE_RATE 44100
#define I2S_BCLK_IO     26
#define I2S_WS_IO       25
#define I2S_DO_IO       22

static const char *TAG = "audio_i2s";

/* ===== File-local (static) state ===== */
static i2s_chan_handle_t tx_chan = NULL;
static bool initialized = false;

/* ===== Public init function ===== */
esp_err_t audio_i2s_init(void)
{
    if (initialized) {
        return ESP_OK;
    }

    esp_err_t err;

    /* Channel config */
    i2s_chan_config_t chan_cfg =
        I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);

    err = i2s_new_channel(&chan_cfg, &tx_chan, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2s_new_channel failed (%s)", esp_err_to_name(err));
        return err;
    }

    /* Standard I2S (Philips) config */
    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
                        I2S_DATA_BIT_WIDTH_16BIT,
                        I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_BCLK_IO,
            .ws   = I2S_WS_IO,
            .dout = I2S_DO_IO,
            .din  = I2S_GPIO_UNUSED,
        },
    };

    err = i2s_channel_init_std_mode(tx_chan, &std_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2s_channel_init_std_mode failed (%s)",
                 esp_err_to_name(err));
        return err;
    }

    err = i2s_channel_enable(tx_chan);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2s_channel_enable failed (%s)",
                 esp_err_to_name(err));
        return err;
    }

    initialized = true;
    ESP_LOGI(TAG, "I2S initialized");
    return ESP_OK;
}


// // Simple HTTP streaming task
// void radio_task(void *arg) {
//     esp_http_client_config_t config = {
//         .url = RADIO_URL,
//         .timeout_ms = 5000
//     };

//     esp_http_client_handle_t client = esp_http_client_init(&config);
//     esp_err_t err = esp_http_client_open(client, 0);
//     if (err != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to open stream: %s", esp_err_to_name(err));
//         vTaskDelete(NULL);
//         return;
//     }

//     uint8_t buffer[1024];
//     int bytes_read;

//     while (1) {
//         bytes_read = esp_http_client_read(client, (char*)buffer, sizeof(buffer));
//         if (bytes_read > 0) {
//             size_t bytes_written;
//             i2s_write(I2S_NUM, buffer, bytes_read, &bytes_written, portMAX_DELAY);
//         } else if (bytes_read == 0) {
//             ESP_LOGI(TAG, "Stream ended");
//             break;
//         } else {
//             ESP_LOGE(TAG, "HTTP read error");
//             break;
//         }
//     }

//     esp_http_client_cleanup(client);
//     vTaskDelete(NULL);
// }

void app_main(void) {
    // Initialize NVS
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    // Connect to Wi-Fi (replace with your Wi-Fi connect function)
    // e.g., example_connect_wifi();

    // i2s_init();


    // Blink 3 times to signal program is now running:
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(LED_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(LED_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(2000));
    gpio_set_level(LED_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(2000));


    const esp_err_t test = audio_i2s_init();
    // xTaskCreate(radio_task, "radio_task", 8192, NULL, 5, NULL);
}
