# TopAMS ESP32 C3 Firmware

## Introduction

This is the firmware for the TopAMS ESP32 C3, designed to control the TopAMS (designed for Bambulab A1 or more 3D printers).

## Features

## Usage

## Using the released firmware

1. Download the latest firmware from the [releases page]()
2. Flash the firmware to your ESP32 C3 using idf.py or flash tool

## Building the firmware

1. Clone the repository

2. Install the ESP-IDF environment
     See the [ESP-IDF documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation) for instructions on how to set up the ESP-IDF environment.

3. Configure the project
  * Open the project configuration menu (`idf.py menuconfig`)
  * Set config by [docs/sdk_config.md](docs/sdk_config.md)

3. Build the project
   * Run `idf.py build` to compile the project

4. Flash the firmware
   * Connect your ESP32 C3 to your computer
   * Run `idf.py -p (PORT) flash` to upload the firmware

## TODO
