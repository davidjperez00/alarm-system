#include "i2s.h"
#include "spi.h"
#include "../drivers/esp32/i2s_device.h"
#include "../drivers/esp32/spi_device.h"
#include "../drivers/esp32/sdcard_device.h"

// for memcmp
#include <string.h>

// esp-idf defines
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Defines for sine function testing
#include "math.h"
#define EXAMPLE_BUFF_SIZE 10000
#define SAMPLE_RATE 44100

// TODO: investigate adding
// buffers that go to dma on the stack?

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

// TODO: move this to some other component/directory
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct __attribute__((packed))
{
    // These 3 fields are required for all wav files.
    char riff_id[4]; // "RIFF"
    uint32_t file_size;
    char wave_id[4]; // "WAVE"
} wav_master_riff_header_t;

typedef struct __attribute__((packed))
{
    // This chunk is required but its size of 16 bytes
    // is only consistent for PCM audio_format.
    // char fmt_id[4];        // "fmt "
    // uint32_t fmt_size;     // 16
    uint16_t audio_format; // 1 = PCM
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
} wav_fmt_min_header_t;

// Note this is also the size of wav_fmt_min_header_t
#define WAV_FMT_MIN_HEADER_BYTES 16

void print_wav_master_riff_header(const wav_master_riff_header_t hdr)
{
    printf("RIFF ID        : %.4s\n", hdr.riff_id);
    printf("File size      : %lu\n", hdr.file_size);
    printf("WAVE ID        : %.4s\n", hdr.wave_id);
}

// TODO: incorporate this somehow
// void print_wav_master_riff_header(const wav_master_riff_header_t hdr)
// {
//     printf("FMT ID         : %.4s\n", hdr.fmt_id);
//     printf("FMT size       : %lu\n", hdr.fmt_size);
// }

void print_wav_fmt_min_header(const wav_fmt_min_header_t hdr)
{
    // printf("FMT ID         : %.4s\n", hdr.fmt_id);
    // printf("FMT size       : %lu\n", hdr.fmt_size);
    printf("Audio format   : %u\n", hdr.audio_format);
    printf("Channels       : %u\n", hdr.num_channels);
    printf("Sample rate    : %lu Hz\n", hdr.sample_rate);
    printf("Byte rate      : %lu\n", hdr.byte_rate);
    printf("Block align    : %u\n", hdr.block_align);
    printf("Bits per samp  : %u\n", hdr.bits_per_sample);
}

void parse_wav_header_contents()
{
}

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

    // Parse the master header that is conistent across all wav files.
    wav_master_riff_header_t master_hdr;
    if (fread(&master_hdr, 1, sizeof(wav_master_riff_header_t), f) != sizeof(wav_master_riff_header_t))
    {
        printf("Failed to read master riff header from wav file\r\n");
        fclose(f);
        return;
    }

    // Print master header
    print_wav_master_riff_header(master_hdr);

    // Allocate buffer for parsing wav file BlocID and BlocSize for each chunk.
    uint8_t chunk_buffer_size = 8; //
    uint8_t chunk_buffer[chunk_buffer_size];
    uint32_t bloc_size = 0;
    // Loop through header chunks until we get to the actual audio data.
    // or end of file is reached.
    while (!feof(f))
    {
        if (fread(&chunk_buffer, 1, chunk_buffer_size, f) != chunk_buffer_size)
        {
            printf("Failed to read WAV chunk \r\n");
            fclose(f);
            return;
        }

        // Get uint32_t conversion of BlocSize:
        bloc_size =
            ((uint32_t)chunk_buffer[4]) |
            ((uint32_t)chunk_buffer[5] << 8) |
            ((uint32_t)chunk_buffer[6] << 16) |
            ((uint32_t)chunk_buffer[7] << 24);

        char bloc_id[5]; // 4 chars + null terminator
        memcpy(bloc_id, chunk_buffer, 4);
        bloc_id[4] = '\0';

        printf("BlocID = %.4s \r\n", bloc_id);
        printf("BlocSize = %ld \r\n", bloc_size);

        if (memcmp(chunk_buffer, "fmt ", 4) == 0)
        {
            if (bloc_size < WAV_FMT_MIN_HEADER_BYTES)
            {
                printf("%s Failed block size is less than 16 bytes\r\n", __func__);
                fclose(f);
                return;
            }

            // Read required fmt header information
            wav_fmt_min_header_t fmt_hdr;
            if (fread(&fmt_hdr, 1, sizeof(wav_fmt_min_header_t), f) != sizeof(wav_fmt_min_header_t))
            {
                printf("Failed to read WAV chunk \r\n");
                fclose(f);
                return;
            }

            // Print header contents
            print_wav_fmt_min_header(fmt_hdr);

            // TODO: Consider different ways to handle this
            // This is the only format that is support by our amplifier setup.
            if (fmt_hdr.audio_format != 1)
            {
                printf("audio_format is invalid, fmt_hdr.audio_format=%d  \r\n", fmt_hdr.audio_format);
                fclose(f);
                return;
            }

            // Skip the remaining fmt data since it's not relevant
            // this is spec-correct according to chatgpt.
            if (fseek(f, bloc_size - WAV_FMT_MIN_HEADER_BYTES, SEEK_CUR))
            {
                printf("Failed to read WAV chunk \r\n");
                fclose(f);
                return;
            }
        }
        // We don't care about this data so we skip it
        else if (memcmp(chunk_buffer, "LIST", 4) == 0)
        {
            if (fseek(f, bloc_size, SEEK_CUR))
            {
                printf("Failed to read WAV chunk \r\n");
                fclose(f);
                return;
            }
        }
        else if (memcmp(chunk_buffer, "data", 4) == 0)
        {
            break;
        }

        // There may be odd numbred block size, since wav files are word alinged
        // there will be a padding byte following this which we can skip.
        if (bloc_size & 1)
        {
            if (fseek(f, 1, SEEK_CUR))
            {
                printf("Failed to increment past padding byte\r\n");
                fclose(f);
                return;
            }
        }
    }

    // Allocate buffer for PCM samples
    size_t buffer_size = 512; // e.g., 512 bytes per read
    int16_t *buffer = malloc(buffer_size * sizeof(int16_t));
    if (!buffer)
    {
        printf("Failed to allocate buffer\n");
        fclose(f);
        return;
    }

    // Read PCM samples in a loop
    size_t read_items;
    while ((read_items = fread(buffer, sizeof(int16_t), buffer_size, f)) > 0)
    {
        // buffer contains PCM data
        // You can now send it to I2S
        printf("buffer[0] = %d\r\n", buffer[0]);
        printf("read_items = %d\r\n", read_items);
        i2s_write(buffer, read_items * sizeof(int16_t));
    }

    free(buffer);
    fclose(f);
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

    read_wav("gs-16b-1c-44100hz.wav");

    // Create the task
    // xTaskCreate(
    //     sample_task,   // task function
    //     "sample_task", // name (for debugging)
    //     4096,          // stack size in bytes
    //     NULL,          // task argument
    //     5,             // priority (0–configMAX_PRIORITIES-1)
    //     NULL           // task handle (optional)
    // );

    // read_wav("gs-16b-1c-44100hz.wav");

    // // Minimal loop
    // while (1)
    // {
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    //     printf("hello world\r\n");
    // }
}
