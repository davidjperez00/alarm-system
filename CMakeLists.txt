cmake_minimum_required(VERSION 3.16)

# If using ESP-IDF, include its project CMake macros
# Otherwise, you can replace this with your own CMake toolchain later
# if(DEFINED ENV{IDF_PATH})
#     include($ENV{IDF_PATH}/tools/cmake/project.cmake)
#     set(ESP_IDF_ENABLED TRUE)
# else()
#     set(ESP_IDF_ENABLED FALSE)
# endif()

# ESP-IDF specific criteria: 
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(alarm_system)

# Add all subdirectories (components / modules)
add_subdirectory(src/subsystems/i2s)
add_subdirectory(src/drivers/esp32)
add_subdirectory(src/app)
