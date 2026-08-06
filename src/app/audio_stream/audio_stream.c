#include "audio_stream.h"
#include "../wav_parser/wav_parser.h"
#include "../../subsystems/i2s/i2s.h"
#include "../../subsystems/spi/spi.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h" // for sine function test

// Outputs additional debug information for deeper investigation
// This should be disabled by default to avoid cluttering output
// and creating program delays.
#define DEBUG_AUDIO_STEAM_VERBOSE true

#define WAV_NUM_SAMPLES 8192
static int16_t wav_buffer[WAV_NUM_SAMPLES];

/**
 * @brief Play wave file over I2S.
 * @param filepath Path to the wav file (including the wav file name).
 * @return true if successful, false on error
 */
bool audio_stream_wav_file(const char *filepath)
{
    FILE *f = fopen(filepath, "rb");
    if (!f)
    {
        printf("ERROR: Failed to open %s\n", filepath);
        return false;
    }

    // Parse header file contents. This leaves the file pointer
    // such that the next read is a data sample from the wav file
    parse_wav_header_contents(f);

    // TODO: Should reconfigure the i2s peripheral based on wav file attributes

    // Read PCM samples in a loop
    size_t read_items;
    // reads 'n' elements, each 'size' bytes long
    while ((read_items = fread(wav_buffer, sizeof(int16_t), WAV_NUM_SAMPLES, f)) > 0)
    {
        // Write to i2s audio device. This function blocks until dma buffers are full
        i2s_write(wav_buffer, read_items * sizeof(int16_t));
    }

    // free(buffer);
    fclose(f);
    return true;
}

// THIS WORKS WITH MY I2S CONFIGURATION
// however, there is a pop about every .7 seconds
// is is likely from phase discontinuity/ empty buffer
#define TEST_SAMPLES 50000
static int16_t test_buffer[TEST_SAMPLES * 2]; // Stereo
void send_sin_wav_part1()
{
#define SAMPLE_RATE 44100

    // Generate 440 Hz tone
    for (int i = 0; i < TEST_SAMPLES; i++)
    {
        // Original working sin wave code
        int16_t sample = (int16_t)(sin(2.0 * M_PI * 432.0 * i / 44100.0) * 5000);
        test_buffer[i * 2] = sample;     // Left
        test_buffer[i * 2 + 1] = sample; // Right

        // // Sending zero's test for power consumption potential issue:
        // test_buffer[i * 2] = 0;     // Left
        // test_buffer[i * 2 + 1] = 0; // Right
    }
    while (1)
    {
        i2s_write(test_buffer, TEST_SAMPLES * 2 * sizeof(int16_t));
    }

    // i2s_write(test_buffer, TEST_SAMPLES * 2 * sizeof(int16_t));
}