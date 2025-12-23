#include "i2s.h"
#include "driver/i2s.h"

static void esp32_i2s_init(int sample_rate, int bits_per_sample, int channels) {
    i2s_config_t cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = sample_rate,
        .bits_per_sample = bits_per_sample == 16 ? I2S_BITS_PER_SAMPLE_16BIT : I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = channels == 2 ? I2S_CHANNEL_FMT_RIGHT_LEFT : I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 4,
        .dma_buf_len = 1024
    };
    i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
}

static const i2s_ops_t esp32_i2s_ops = {
    .init = esp32_i2s_init,
};

void i2s_driver_register_esp32(void) {
    i2s_register_ops(&esp32_i2s_ops);
}
