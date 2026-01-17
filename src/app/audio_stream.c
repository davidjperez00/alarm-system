#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>

#include "audio_stream.h"
#include "ring_buffer.h"
#include "sdcard_interface.h"
#include "i2s.h"
#include "../drivers/esp32/i2s_device.h"

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

#define I2S_WRITE_BUFFER_SAMPLES 512 // Write 2048 samples (1024 L/R pairs) at a time
// NOTE: This is int16 so bytes is this * 2
int16_t i2s_buffer[I2S_WRITE_BUFFER_SAMPLES];

/**
 * @brief Task that reads from SD card and writes to ring buffer
 * @param pvParameters FILE pointer to the WAV file
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

    printf("SD reader ring buffer population started \r\n");
    while (1)
    {
        // Check if ring buffer has enough space (need space for stereo)
        size_t space = ring_buffer_space(&audio_ring_buffer);
        if (space < SD_READ_BUFFER_SAMPLES)
        {
            // Buffer is full, wait a bit
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

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

        printf("DEBUG: %s %zu writtent to ring buf \r\n", __func__, written);

        if (written < items_read)
        {
            printf("WARNING: Ring buffer full, dropped %zu samples\n",
                   items_read - written);
        }

        // Small delay to yield to other tasks
        vTaskDelay(1);
    }

    fclose(f);
    printf("SD reader task exiting\n");
    sd_reader_task_handle = NULL;
    vTaskDelete(NULL);
}

/**
 * @brief Task that reads from ring buffer and writes to I2S
 * @param pvParameters Unused
 */
void i2s_writer_task(void *pvParameters)
{
    (void)pvParameters; // Unused

    size_t total_samples_written = 0;

    printf("I2S writer task started\n");

    while (1)
    {
        // Check if we have enough data in ring buffer
        size_t available = ring_buffer_available(&audio_ring_buffer);

        if (available >= I2S_WRITE_BUFFER_SAMPLES)
        {
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
        }
        else if (sd_read_complete)
        {
            // SD reading is done, flush remaining data
            if (available > 0)
            {
                printf("Flushing remaining %zu samples\n", available);
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
            printf("Playback complete. Total samples written: %zu\n",
                   total_samples_written);
            break;
        }
        else
        {
            // Not enough data yet and SD still reading, wait
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }

    printf("I2S writer task exiting\n");
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
        printf("Failed to open %s\n", path);
        return;
    }

    // Parse WAV header
    parse_wav_header_contents(f);

    // Initialize ring buffer
    if (!ring_buffer_init(&audio_ring_buffer))
    {
        printf("Failed to initialize ring buffer\n");
        fclose(f);
        return;
    }

    // Reset completion flag
    sd_read_complete = false;

    printf("Starting audio playback: %s\n", filename);

    // Create SD reader task (lower priority)
    BaseType_t result = xTaskCreatePinnedToCore(
        sd_reader_task,         // Task function
        "SD_Reader",            // Task name
        4096,                   // Stack size (bytes)
        (void *)f,              // Pass file pointer as parameter
        5,                      // Priority (lower)
        &sd_reader_task_handle, // Task handle
        0                       // cpu core 0
    );

    if (result != pdPASS)
    {
        printf("Failed to create SD reader task\n");
        ring_buffer_deinit(&audio_ring_buffer);
        fclose(f);
        return;
    }

    // Create I2S writer task (higher priority for smooth playback)
    result = xTaskCreatePinnedToCore(
        i2s_writer_task,         // Task function
        "I2S_Writer",            // Task name
        4096,                    // Stack size (bytes)
        NULL,                    // No parameters
        5,                       // Priority (higher)
        &i2s_writer_task_handle, // Task handle
        1                        // cpu core 1
    );

    if (result != pdPASS)
    {
        printf("Failed to create I2S writer task\n");
        // Cancel SD reader task
        if (sd_reader_task_handle != NULL)
        {
            vTaskDelete(sd_reader_task_handle);
        }
        ring_buffer_deinit(&audio_ring_buffer);
        fclose(f);
        return;
    }

    printf("Audio tasks created successfully\n");

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