idf_component_register(
    SRCS main.c sdcard_interface.c ring_buffer.c
    INCLUDE_DIRS .
    REQUIRES 
        i2s # My project directory
        esp32 # my project directory
)
