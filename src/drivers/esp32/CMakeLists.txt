idf_component_register(
    SRCS
        i2s_device.c
        spi_device.c # move this
        sdcard_device.c
    INCLUDE_DIRS .
    REQUIRES
        i2s # My project directory
        spi # My custom project
        esp_driver_i2s
        esp_driver_spi
        esp_driver_gpio
        esp_driver_sdspi
        fatfs
)
