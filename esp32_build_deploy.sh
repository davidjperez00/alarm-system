source $IDF_PATH/export.sh


# Set my board specs as environment variables:
# could also specify
export IDF_TARGET=esp32
export ESPPORT=/dev/ttyUSB0
export ESPBAUD=115200


# Set the serial port config
idf.py build

idf.py flash
