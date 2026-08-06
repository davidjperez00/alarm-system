idf_component_register(
    SRCS main.c
    INCLUDE_DIRS .
            # "${CMAKE_CURRENT_SOURCE_DIR}"  # Only this exact directory

    REQUIRES 
        i2s # My project directory
        esp32 # my project directory
        audio_amp  # my project directory
)
