#!/bin/bash
# Description: WSL host machine ESP32 setup

# Terminal Color Coding
GREEN="\033[0;32m"
RESET="\033[0m"

# Update system file manager
sudo apt update && sudo apt upgrade -y

echo "=== Installing dependencies ==="
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

echo "=== Installing ESP-IDF tools ==="
# Setup Folder and clone esp-idf
ESP_DIR=~/esp
ESP_IDF_DIR=$ESP_DIR/esp-idf
mkdir -p ~/esp
cd ~/esp
if [ ! -d "$ESP_IDF_DIR" ]; then
    echo "=== Cloning ESP-IDF repository ==="
    git clone --recursive https://github.com/espressif/esp-idf.git

    #Run the pythong packages install script
    cd ~/esp/esp-idf
    ./install.sh esp32

else
    echo "=== ESP-IDF already exists. Pulling latest changes ==="
    cd esp-idf
    git pull
    git submodule update --init --recursive
fi

# --- Set up environment variables ---
echo "=== Setting up environment variables ==="
if ! grep -q 'IDF_PATH' ~/.bashrc; then
    echo "export IDF_PATH=$ESP_IDF_DIR" >> ~/.bashrc
    echo 'export PATH="$PATH:$HOME/.local/bin"' >> ~/.bashrc
fi

echo -e "${GREEN} System Installation Successfully Complete ${RESET}"