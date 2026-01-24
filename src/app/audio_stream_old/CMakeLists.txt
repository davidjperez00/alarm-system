idf_component_register(
    SRCS ring_buffer.c audio_stream_ring.c
    INCLUDE_DIRS .
            # "${CMAKE_CURRENT_SOURCE_DIR}"  # Only this exact directory

    REQUIRES 
        i2s # My project directory
        esp32 # my project directory
        esp_timer # DEBUG WATCHDOG ISSUES
)
