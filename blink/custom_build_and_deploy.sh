# Description: Set the device config and Serial port configuration.
#    Then, Build idf project and flash the project to the device. 
# 
# Date: 12/16/2025

# Source IDF environment variables
# This sets up path for idf.py programmer usign their custom env setup:
source $IDF_PATH/export.sh


# Set my board specs as environment variables:
# could also specify
export IDF_TARGET=esp32
export ESPPORT=/dev/ttyUSB0
export ESPBAUD=115200


# Set the serial port config
idf.py build

idf.py flash

