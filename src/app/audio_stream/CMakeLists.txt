idf_component_register(
    SRCS
        audio_stream.c
    INCLUDE_DIRS .
    REQUIRES
        i2s # My project directory
        spi # My custom project
        wav_parser # custom
        esp_driver_i2s
        esp_driver_spi
        # esp_driver_gpio
        # esp_driver_sdspi
        # fatfs
)
