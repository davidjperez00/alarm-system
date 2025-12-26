#include "i2s.h"
#include "../drivers/esp32/i2s_device.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Defines for sine function testing
#include "math.h"
#define EXAMPLE_BUFF_SIZE 10000
#define SAMPLE_RATE 44000

static void send_sine_wave()
{
    int16_t *w_buf = calloc(EXAMPLE_BUFF_SIZE / 2, sizeof(int16_t));
    assert(w_buf);

    int samples = EXAMPLE_BUFF_SIZE / 2; // '/2' idf.is for bytes to int16

    int16_t frequency = 440; // 440hz
    int16_t amplitude_max = 200;

    for (int i = 0; i < samples; i++)
    {
        w_buf[i] = (int16_t)(amplitude_max * sin(2 * M_PI * frequency * i / SAMPLE_RATE));
    }

    i2s_write(w_buf, EXAMPLE_BUFF_SIZE);
}

void app_main(void)
{
    // Register ESP32 I2S driver
    i2s_driver_register_esp32();

    // Init I2S with basic configuration
    i2s_init(SAMPLE_RATE, 16, 2);

    send_sine_wave();

    // // Minimal loop
    // while (1)
    // {
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    //     printf("hello world\r\n");
    // }
}
