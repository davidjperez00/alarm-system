#include "i2s.h"
#include "spi.h"
#include "../drivers/esp32/i2s_device.h"
#include "../drivers/esp32/spi_device.h"
#include "../drivers/esp32/sdcard_device.h"
#include "sdcard_interface.h"

// for memcmp
#include <string.h>

// esp-idf defines
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Defines for sine function testing
#include "math.h"
#define SAMPLE_RATE 44100

// wave file from sd card to read
// #define WAV_FILE_NAME "gs-16b-1c-44100hz.wav"
// #define WAV_FILE_NAME "gs-16b-2c-44100hz.wav"
#define WAV_FILE_NAME "3min-ff-16b-2c-44100hz.wav"

// TODO: move this to some other component/directory

#include <stdlib.h>

// THIS WORKS WITH MY I2S CONFIGURATION
// however, there is a pop about every .7 seconds
// is is likely from phase discontinuity/ empty buffer
// #define TEST_SAMPLES 30000
// static int16_t test_buffer[TEST_SAMPLES * 2]; // Stereo
// static void send_sin_wav_part1()
// {

//     // Generate 440 Hz tone
//     for (int i = 0; i < TEST_SAMPLES; i++)
//     {
//         int16_t sample = (int16_t)(sin(2.0 * M_PI * 440.0 * i / 44100.0) * 10000);
//         test_buffer[i * 2] = sample;     // Left
//         test_buffer[i * 2 + 1] = sample; // Right
//     }
//     while (1)
//     {
//         i2s_write(test_buffer, TEST_SAMPLES * 2 * sizeof(int16_t));
//     }
// }

#define WAV_NUM_SAMPLES 8192
#define ZERO_BUF_SAMPLES 512
static int16_t wav_buffer[WAV_NUM_SAMPLES];      // Stereo
static int16_t zero_buf[ZERO_BUF_SAMPLES] = {0}; // Stereo
// IMPLEMENTATION FROM 1/14
void read_wav(const char *filename)
{
    char path[64];
    snprintf(path, sizeof(path), "/AUDIOSDCARD/%s", filename);

    FILE *f = fopen(path, "rb");
    if (!f)
    {
        printf("Failed to open %s\n", path);
        return;
    }

    // Parse header file contents. This leaves the file pointer
    // such that the next read is a data sample from the wave file
    parse_wav_header_contents(f);

    // Read PCM samples in a loop
    size_t read_items;
    // reads 'n' elements, each 'size' bytes long
    while ((read_items = fread(wav_buffer, sizeof(int16_t), WAV_NUM_SAMPLES, f)) > 0)
    {
        // printf("buffer[0] = %d\r\n", buffer[0]);
        printf("read_items = %d\r\n", read_items);
        i2s_write(wav_buffer, read_items * sizeof(int16_t));

        // INTENTIONALLY FEED SILENCE WHILE SD IS BUSY
        i2s_write(zero_buf, ZERO_BUF_SAMPLES * sizeof(int16_t));
    }

    // free(buffer);
    fclose(f);

    // ORIGINAL ATTEMPT:
    // Allocate buffer for PCM samples
    // size_t num_samples = 30000; // e.g., 512 bytes per read
    // int16_t *buffer = malloc(num_samples * sizeof(int16_t));
    // if (!buffer)
    // {
    //     printf("Failed to allocate buffer\n");
    //     fclose(f);
    //     return;
    // }

    // // Testing by writing to a file:
    // FILE *f_test = fopen("/AUDIOSDCARD/test.raw", "wb");
    // if (!f_test)
    // {
    //     printf("Failed to open file\n");
    //     return;
    // }
    // size_t read_items_test;
    // while ((read_items_test = fread(wav_buffer, sizeof(int16_t), WAV_NUM_SAMPLES, f)) > 0)
    // {
    //     // buffer contains PCM data
    //     // You can now send it to I2S
    //     // printf("buffer[0] = %d\r\n", buffer[0]);
    //     printf("read_items_test = %d\r\n", read_items_test);
    //     fwrite(wav_buffer, sizeof(int16_t), read_items_test, f_test);
    // }

    // printf("finished writing to f_test\r\n");
    // fclose(f);
    // fclose(f_test);

    // // Claude:
    // //  Allocate stereo buffer (2x size for L+R channels)
    // size_t mono_samples = 2000; // Number of mono samples per chunk
    // int16_t *stereo_buffer = malloc(mono_samples * 2 * sizeof(int16_t));
    // int16_t *mono_buffer = malloc(mono_samples * sizeof(int16_t));

    // if (!stereo_buffer || !mono_buffer)
    // {
    //     printf("Failed to allocate buffers\n");
    //     fclose(f);
    //     return;
    // }

    // size_t read_items;
    // while ((read_items = fread(mono_buffer, sizeof(int16_t), mono_samples, f)) > 0)
    // {
    //     // Duplicate mono to stereo: [L, R, L, R, L, R...]
    //     for (size_t i = 0; i < read_items; i++)
    //     {
    //         stereo_buffer[i * 2] = mono_buffer[i];     // Left channel
    //         stereo_buffer[i * 2 + 1] = mono_buffer[i]; // Right channel (duplicate)
    //     }

    //     // // debug printing:
    //     // for (size_t i = 0; i < read_items * 2; i++)
    //     // {
    //     //     printf("(%d, %d)", i, stereo_buffer[i]);

    //     //     if (i % 20 == 0)
    //     //     {
    //     //         printf("\r\n");
    //     //     }
    //     // }

    //     // Write stereo data to I2S
    //     printf("read_items = %u\r\n", read_items);
    //     i2s_write(stereo_buffer, read_items * 2 * sizeof(int16_t));
    // }

    // free(stereo_buffer);
    // free(mono_buffer);
    // fclose(f);
}

void sample_task(void *arg)
{
    read_wav("gs-16b-1c-44100hz.wav");
}

void app_main(void)
{
    printf("HELLO STARTING MAIN APPLICATION \r\n\r\n");
    /* BSP LEVEL INITIALIZATIONS*/
    // Register ESP32 I2S driver
    i2s_driver_register_esp32();
    // Register ESP32 SPI driver
    driver_esp32_spi_register_ops();

    // Init I2S with basic configuration
    i2s_init(SAMPLE_RATE, 16, 2);
    // Init SPI with basic configuration
    spi_init();

    /* APPLICATION INITIALIZATIONS*/
    // Initialize the sdcard esp idf api
    driver_esp32_sdcard_init();

    // send_sine_wave();
    // send_sin_wav_part1();

    read_wav(WAV_FILE_NAME);

    // Create the task
    // xTaskCreate(
    //     sample_task,   // task function
    //     "sample_task", // name (for debugging)
    //     4096,          // stack size in bytes
    //     NULL,          // task argument
    //     5,             // priority (0–configMAX_PRIORITIES-1)
    //     NULL           // task handle (optional)
    // );

    // // Minimal loop
    // while (1)
    // {
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    //     printf("hello world\r\n");
    // }
}
