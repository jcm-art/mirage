Mirage Node
===========

# Overview

This module contains the code for the Mirage Node, which is the ESP32-based microcontroller that controls the LED strips and communicates with the Mirage Server.

# Usage Instructions

## Setup

To 

## Over the Air Updates

The Mirage Node supports over the air (OTA) updates. This means that you can update the firmware on the microcontroller without having to physically connect it to a computer. This is useful for updating the firmware on the microcontroller after it has been installed in the sculpture.

To enable OTA updates, a base image with OTA updates enabled must first be installed. To do this, remove the following lines from the `platformio.ini` file:

    upload_protocol = espota
    upload_port = 192.168.86.36

Then, install the base firmware image via the serial interface. After the base OTA image is installed, the ESP32 will broadcast it's IP address on the serial interface. This can then be used to upload new firmware images over the air by adding the two lines above back to the platformio.ini file and rebuilding the image.

Note that the Network SSID and Password for the node are hardcoded; if either is changed, the firmware will need to be reflashed via the serial interface.

Note that OTA updates are significantly slower than serial updates; for development, it may be desireable to use the serial interface for updates.