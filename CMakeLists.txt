cmake_minimum_required(VERSION 3.16)

# If using ESP-IDF, include its project CMake macros
# Otherwise, you can replace this with your own CMake toolchain later
# if(DEFINED ENV{IDF_PATH})
#     include($ENV{IDF_PATH}/tools/cmake/project.cmake)
#     set(ESP_IDF_ENABLED TRUE)
# else()
#     set(ESP_IDF_ENABLED FALSE)
# endif()



# Tell ESP-IDF where to find "components" in src/
set(EXTRA_COMPONENT_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/src/subsystems/i2s
    ${CMAKE_CURRENT_LIST_DIR}/src/subsystems/spi
    ${CMAKE_CURRENT_LIST_DIR}/src/drivers/esp32
    ${CMAKE_CURRENT_LIST_DIR}/src/app
    ${CMAKE_CURRENT_LIST_DIR}/src/app/wav_parser
    ${CMAKE_CURRENT_LIST_DIR}/src/app/audio_stream
    ${CMAKE_CURRENT_LIST_DIR}/src/app/audio_stream_old
)

# ESP-IDF specific criteria: 
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(alarm_system)

# Add all subdirectories (components / modules)
# add_subdirectory(src/subsystems/i2s)
# add_subdirectory(src/drivers/esp32)
# add_subdirectory(src/app)
