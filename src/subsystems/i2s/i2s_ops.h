

typedef struct i2s_ops {
    void (*init)(int sample_rate, int bits_per_sample, int channels);
} i2s_ops_t;
