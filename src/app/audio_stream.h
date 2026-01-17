/**
 * @brief Task that reads from SD card and writes to ring buffer
 * @param pvParameters FILE pointer to the WAV file
 */
void sd_reader_task(void *pvParameters);

/**
 * @brief Task that reads from ring buffer and writes to I2S
 * @param pvParameters Unused
 */
void i2s_writer_task(void *pvParameters);
/**
 * @brief Start playback of a WAV file using ring buffer and tasks
 * @param filename Name of the WAV file (without path)
 */
void read_wav_with_ring_buffer(const char *filename);

/**
 * @brief Check if audio playback is still active
 * @return true if tasks are still running, false if complete
 */
bool is_audio_playing(void);

/**
 * @brief Stop audio playback and clean up
 */
void stop_audio_playback(void);