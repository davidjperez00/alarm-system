#include "i2s.h"
#include "../drivers/esp32/i2s_device.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void) {
    // Register ESP32 I2S driver
    i2s_driver_register_esp32();

    // Init I2S with basic configuration
    i2s_init(44100, 16, 2);

    // Minimal loop
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("hello world\r\n");
    }
}
