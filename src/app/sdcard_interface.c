#include "sdcard_interface.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// These 3 fields are required for all wav files.
typedef struct __attribute__((packed))
{
    char riff_id[4]; // "RIFF"
    uint32_t file_size;
    char wave_id[4]; // "WAVE"
} wav_master_riff_header_t;

// This chunk ("fmt ") is required but its size of 16 bytes
// is only consistent for PCM audio_format.
typedef struct __attribute__((packed))
{
    uint16_t audio_format; // 1 = PCM
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
} wav_fmt_min_header_t;

// Note this min size of wav_fmt_min_header_t
#define WAV_FMT_MIN_HEADER_BYTES 16

void print_wav_master_riff_header(const wav_master_riff_header_t hdr)
{
    printf("RIFF ID        : %.4s\n", hdr.riff_id);
    printf("File size      : %lu\n", hdr.file_size);
    printf("WAVE ID        : %.4s\n", hdr.wave_id);
}

void print_wav_fmt_min_header(const wav_fmt_min_header_t hdr)
{
    printf("Audio format     : %u\n", hdr.audio_format);
    printf("Channels         : %u\n", hdr.num_channels);
    printf("Sample rate      : %lu Hz\n", hdr.sample_rate);
    printf("Byte rate        : %lu\n", hdr.byte_rate);
    printf("Block align      : %u\n", hdr.block_align);
    printf("Bits per sample  : %u\n", hdr.bits_per_sample);
}

// void print_wav_generic_chunk_header()
// {
//     printf("BlocID = %.4s \r\n", chunk_buffer);
//     printf("BlocSize = %ld \r\n", bloc_size);
// }

void parse_wav_header_contents(FILE *f)
{
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

    // Loop and read chunkID and size to parse wav file.

    while (fread(chunk_buffer, 1, chunk_buffer_size, f) == chunk_buffer_size)
    {
        // Get uint32_t conversion of BlocSize:
        memcpy(&bloc_size, chunk_buffer + 4, sizeof(bloc_size));

        printf("BlocID = %.4s \r\n", chunk_buffer);
        printf("BlocSize = %ld \r\n", bloc_size);

        if (memcmp(chunk_buffer, "fmt ", 4) == 0)
        {
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

            // TODO: MAY NEED TO DELETE THIS LATER (only for specific files)
            if (bloc_size != WAV_FMT_MIN_HEADER_BYTES)
            {
                if (fseek(f, bloc_size - WAV_FMT_MIN_HEADER_BYTES, SEEK_CUR) != 0)
                {
                    printf("Failed to read WAV chunk \r\n");
                    fclose(f);
                    return;
                }
            }
        }
        // We don't care about this data so we skip it
        else if (memcmp(chunk_buffer, "LIST", 4) == 0)
        {
            if (fseek(f, bloc_size, SEEK_CUR) != 0)
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
        else
        {
            printf("CAUTION: unexpected BlockID found \r\n");
            if (fseek(f, bloc_size, SEEK_CUR) != 0)
            {
                printf("Failed to read WAV chunk \r\n");
                fclose(f);
                return;
            }
        }

        // There may be odd numbred block size, since wav files are word alinged
        // there will be a padding byte following this which we can skip.
        if (bloc_size & 1)
        {
            if (fseek(f, 1, SEEK_CUR) != 0)
            {
                printf("Failed to increment past padding byte\r\n");
                fclose(f);
                return;
            }
        }
    }
}