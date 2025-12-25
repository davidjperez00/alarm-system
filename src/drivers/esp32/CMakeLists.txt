idf_component_register(
    SRCS i2s_device.c
    INCLUDE_DIRS .
    REQUIRES
        i2s # My project directory
        esp_driver_i2s
 
)
