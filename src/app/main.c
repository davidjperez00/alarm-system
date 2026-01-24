#include "i2s.h"
#include "spi.h"
#include "../drivers/esp32/i2s_device.h"
#include "../drivers/esp32/spi_device.h"
#include "../drivers/esp32/sdcard_device.h"
#include "audio_stream/audio_stream.h"

// I2S subsystem defines, TODO: eventually should be changed to
// configure i2s channel based on wav file data.
#define SAMPLE_RATE 44100

// for memcmp
#include <string.h>

// esp-idf defines
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// wave file from sd card to read
// #define WAV_FILE_NAME "gs-16b-1c-44100hz.wav"
// #define WAV_FILE_NAME "3min-ff-16b-2c-44100hz.wav"

// String concatenation for audio file to play
#define WAV_FILE_NAME   \
    SD_CARD_MOUNT_POINT \
    SD_CARD_MAIN_DIRECTORY "gs-16b-2c-44100hz.wav"

void app_main(void)
{
    /* BSP LEVEL INITIALIZATIONS*/
    // Register ESP32 I2S driver
    i2s_driver_register_esp32();
    // Register ESP32 SPI driver
    driver_esp32_spi_register_ops();

    // Init I2S with basic configuration
    // TODO: This should probably be set for the specific wav file
    // that is going to be played.
    i2s_init(SAMPLE_RATE, 16, 2);

    // Init SPI with basic configuration
    spi_init();

    /* APPLICATION INITIALIZATIONS*/
    // Initialize the sdcard esp idf api
    driver_esp32_sdcard_init();

    // send_sine_wave();
    // send_sin_wav_part1();
    audio_stream_wav_file(WAV_FILE_NAME);

    // Used to prevent popping when audio file finishes playing
    i2s_deinit();
}
