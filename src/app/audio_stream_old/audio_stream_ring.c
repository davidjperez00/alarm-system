#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>

#include "audio_stream_ring.h"
#include "ring_buffer.h"
#include "../wav_parser/wav_parser.h"
#include "i2s.h"
#include "../../drivers/esp32/i2s_device.h"

// DEBUG WATCHDOG ISSUES:
#include "esp_timer.h"

// Ring buffer shared between tasks
static ring_buffer_t audio_ring_buffer;

// Task handles (so we can check if tasks are running)
static TaskHandle_t sd_reader_task_handle = NULL;
static TaskHandle_t i2s_writer_task_handle = NULL;

// Flag to signal end of file
static volatile bool sd_read_complete = false;

// Sd card read elements buffer
// NOTE: This is int16 so bytes is this * 2
#define SD_READ_BUFFER_SAMPLES 4096 // Read 4096 mono samples at a time
static int16_t sd_reader_buffer[SD_READ_BUFFER_SAMPLES];

#define I2S_WRITE_BUFFER_SAMPLES 1024 // Write 2048 samples (1024 L/R pairs) at a time
// NOTE: This is int16 so bytes is this * 2
int16_t i2s_buffer[I2S_WRITE_BUFFER_SAMPLES];

/**
 * @brief Task that reads from SD card and writes to ring buffer
 * @param pvParameters FILE pointer to the WAV file
 */

// Debug Timing insights:
/*
Setup:
sd_buf = 1024 (samples read each read)
clock_max_freq = 10MHz

average time to read sd card and write to ring buf: 9.7ms
SD last read: 10.3ms

*/
void sd_reader_task(void *pvParameters)
{
    FILE *f = (FILE *)pvParameters;

    if (f == NULL)
    {
        printf("ERROR: sd_reader_task - NULL file pointer\n");
        vTaskDelete(NULL);
        return;
    }

    sd_read_complete = false;
    size_t total_samples_read = 0;

    // DEBUG WATCHDOG ISSUES:
    // Measure read time
    int64_t start = 0;
    int64_t total_read_time = 0;
    size_t read_count = 0;

    printf("DEBUG %s Task main loop starting\r\n", __func__);
    while (1)
    {
        // Check if ring buffer has enough space (need space for stereo)
        size_t space = ring_buffer_space(&audio_ring_buffer);
        if (space < SD_READ_BUFFER_SAMPLES)
        {
            // Buffer is full, wait a bit
            printf("DEBUG: %s ring buff space less than samples read, space= %u \r\n", __func__, space);
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        // DEBUG WATCHDOG ISSUES:
        start = esp_timer_get_time();

        // Read samples from SD card
        size_t items_read = fread(sd_reader_buffer, sizeof(int16_t),
                                  SD_READ_BUFFER_SAMPLES, f);

        if (items_read == 0)
        {
            // End of file
            printf("SD read complete. Total samples read: %zu\n", total_samples_read);
            sd_read_complete = true;
            break;
        }

        total_samples_read += items_read;

        // Write stereo data to ring buffer
        size_t written = ring_buffer_write(&audio_ring_buffer,
                                           sd_reader_buffer,
                                           items_read);

        printf("DEBUG: %s ring buffer elements written: %zu \r\n", __func__, written);

        if (written < items_read)
        {
            printf("WARNING: Ring buffer full, dropped %zu samples\n",
                   items_read - written);
        }

        //  DEBUG Watchdog issue
        int64_t elapsed = esp_timer_get_time() - start;

        total_read_time += elapsed;
        read_count++;

        if (read_count % 10 == 0)
        {
            printf("DEBUG: %s SD Average read time: %lld μs\n", __func__, total_read_time / read_count);
            printf("DEBUG: %s SD Last read: %lld μs\n", __func__, elapsed);

            total_read_time = 0;
            read_count = 0;
        }

        // Small delay to yield to other tasks
        // vTaskDelay(100);
        printf("DEBUG: %s SD CARD DELAY x ms \r\n", __func__);
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    fclose(f);
    printf("DEBUG: %s SD reader task exiting\n", __func__);
    sd_reader_task_handle = NULL;
    vTaskDelete(NULL);
}

/**
 * @brief Task that reads from ring buffer and writes to I2S
 * @param pvParameters Unused
 */
/*
// Debug Timing insights:
Setup:
i2s_buf = 512 (read from ring and wrote to i2s)
clock_max_freq = 44.1kHz

average time: 4.233ms
last read: 4.238ms
*/
void i2s_writer_task(void *pvParameters)
{
    (void)pvParameters; // Unused

    size_t total_samples_written = 0;

    // DEBUG WATCHDOG ISSUES:
    // Measure read time
    int64_t start = 0;
    int64_t total_read_time = 0;
    size_t read_count = 0;

    printf("DEBUG: %s task main loop started\r\n", __func__);
    while (1)
    {
        // Check if we have enough data in ring buffer
        size_t available = ring_buffer_available(&audio_ring_buffer);

        if (available >= I2S_WRITE_BUFFER_SAMPLES)
        {
            // DEBUG WATCHDOG ISSUES:
            start = esp_timer_get_time();

            // Read from ring buffer
            size_t samples_read = ring_buffer_read(&audio_ring_buffer,
                                                   i2s_buffer,
                                                   I2S_WRITE_BUFFER_SAMPLES);

            if (samples_read > 0)
            {
                // Write to I2S
                i2s_write(i2s_buffer, samples_read * sizeof(int16_t));
                total_samples_written += samples_read;
                printf("DEBUG: %s %zu read from ring buf \r\n", __func__, samples_read);
            }

            // DEBUG WATCHDOG ISSUES:
            int64_t elapsed = esp_timer_get_time() - start;

            total_read_time += elapsed;
            read_count++;

            if (read_count % 10 == 0)
            {
                printf("DEBUG: %s I2S Average read time: %lld us\r\n", __func__, total_read_time / read_count);
                printf("DEBUG: %s I2S Last read: %lld us\r\n", __func__, elapsed);

                total_read_time = 0;
                read_count = 0;
            }
        }
        else if (sd_read_complete)
        {
            // SD reading is done, flush remaining data
            if (available > 0)
            {
                printf("DEBUG: %s Flushing remaining %zu samples\n", __func__, available);
                size_t samples_read = ring_buffer_read(&audio_ring_buffer,
                                                       i2s_buffer,
                                                       available);
                if (samples_read > 0)
                {
                    i2s_write(i2s_buffer, samples_read * sizeof(int16_t));
                    total_samples_written += samples_read;
                }
            }

            // All done
            printf("DEBUG: %s Playback complete. Total samples written: %zu\r\n", __func__,
                   total_samples_written);
            break;
        }

        // Delay for 1 tick for a OR b:
        // a. Not enough data yet, SD still reading
        // b. Satisfy watchdog
        // vTaskDelay(100);
        // printf("DEBUG: %s I2S DELAY x ms \r\n", __func__);
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    printf("DEBUG: %s I2S writer task exiting\r\n", __func__);
    i2s_writer_task_handle = NULL;
    vTaskDelete(NULL);
}

/**
 * @brief Start playback of a WAV file using ring buffer and tasks
 * @param filename Name of the WAV file (without path)
 */
void read_wav_with_ring_buffer(const char *filename)
{
    char path[64];
    snprintf(path, sizeof(path), "/AUDIOSDCARD/%s", filename);

    FILE *f = fopen(path, "rb");
    if (!f)
    {
        printf("ERROR: %s Failed to open %s\n", __func__, path);
        return;
    }

    // Parse WAV header
    parse_wav_header_contents(f);

    // Initialize ring buffer
    if (!ring_buffer_init(&audio_ring_buffer))
    {
        printf("ERROR: %s Failed to initialize ring buffer\r\n", __func__);
        fclose(f);
        return;
    }

    // Reset completion flag
    sd_read_complete = false;

    printf("DEBUG: %s Starting audio playback: %s\n", __func__, filename);

    // Create SD reader task (lower priority)
    BaseType_t result = xTaskCreatePinnedToCore(
        sd_reader_task,         // Task function
        "SD_Reader",            // Task name
        4096,                   // Stack size (bytes)
        (void *)f,              // Pass file pointer as parameter
        2,                      // Priority (lower)
        &sd_reader_task_handle, // Task handle
        1                       // cpu core 0
    );

    if (result != pdPASS)
    {
        printf("ERROR: %s Failed to create SD reader task\r\n", __func__);
        ring_buffer_deinit(&audio_ring_buffer);
        fclose(f);
        return;
    }

    printf("DEBUG: %s SD_Reader task successfully initialized \r\n", __func__);

    // CAVEAT: Before starting the writer task, we want to preload
    // the ring buffer, this is to prevent the i2s writer draining the
    // entire buffer and the sd reader and i2s writer go back
    // and fourth causing popping sounds during audio playback.
    printf("DEBUG: %s, ring buffer pre-fill started \r\n", __func__);
    while (ring_buffer_available(&audio_ring_buffer) < RING_BUFFER_SIZE &&
           !sd_read_complete)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    printf("DEBUG: %s, ring buffer pre-fill completed \r\n", __func__);

    // Create I2S writer task (higher priority for smooth playback)
    result = xTaskCreatePinnedToCore(
        i2s_writer_task,         // Task function
        "I2S_Writer",            // Task name
        4096,                    // Stack size (bytes)
        NULL,                    // No parameters
        2,                       // Priority (higher)
        &i2s_writer_task_handle, // Task handle
        1                        // cpu core 1
    );

    if (result != pdPASS)
    {
        printf("ERROS: %s Failed to create I2S writer task\r\n", __func__);
        // Cancel SD reader task
        if (sd_reader_task_handle != NULL)
        {
            vTaskDelete(sd_reader_task_handle);
        }
        ring_buffer_deinit(&audio_ring_buffer);
        fclose(f);
        return;
    }

    printf("DEBUG: %s I2S_Writer task successfully initialized \r\n", __func__);

    // Tasks will run independently now
    // They will clean themselves up when done
}

/**
 * @brief Check if audio playback is still active
 * @return true if tasks are still running, false if complete
 */
bool is_audio_playing(void)
{
    return (sd_reader_task_handle != NULL || i2s_writer_task_handle != NULL);
}

/**
 * @brief Stop audio playback and clean up
 */
void stop_audio_playback(void)
{
    printf("Stopping audio playback\n");

    // Delete tasks if they're still running
    if (sd_reader_task_handle != NULL)
    {
        vTaskDelete(sd_reader_task_handle);
        sd_reader_task_handle = NULL;
    }

    if (i2s_writer_task_handle != NULL)
    {
        vTaskDelete(i2s_writer_task_handle);
        i2s_writer_task_handle = NULL;
    }

    // Clean up ring buffer
    ring_buffer_deinit(&audio_ring_buffer);

    printf("Audio playback stopped\n");
}