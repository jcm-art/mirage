Mirage Node
===========

# Overview

This module contains the code for the Mirage Node, which is the ESP32-based microcontroller that controls the LED strips and communicates with the Mirage Server.

# Usage Instructions

## Setup

To 

# Platform Variants

## Wifi Platform

### Provisioning

TBD - need to configure IP address, network, and password and flash via USB if not on correct network

### Over the Air Updates

The Mirage Node supports over the air (OTA) updates. This means that you can update the firmware on the microcontroller without having to physically connect it to a computer. This is useful for updating the firmware on the microcontroller after it has been installed in the sculpture.

To enable OTA updates, a base image with OTA updates enabled must first be installed. To do this, remove the following lines from the `platformio.ini` file:

    upload_protocol = espota
    upload_port = 192.168.86.36

Then, install the base firmware image via the serial interface. After the base OTA image is installed, the ESP32 will broadcast it's IP address on the serial interface. This can then be used to upload new firmware images over the air by adding the two lines above back to the platformio.ini file and rebuilding the image.

Note that the Network SSID and Password for the node are hardcoded; if either is changed, the firmware will need to be reflashed via the serial interface.

Note that OTA updates are significantly slower than serial updates; for development, it may be desireable to use the serial interface for updates.


## Ethernet Platform

### Network Setup

First, a network needs to be set up to enable the Mirage Node to communicate with the Mirage Server. This can be done by connecting the Mirage Node to a network via an Ethernet cable. Network addresses must be statically assigned to the Mirage Node dye to current lack of DHCP support in the project scope / field deployment.

One option for doing this is to configure a static USB to Ethernet adapter that can be plugged into the host compute. My settings for the first USB ethernet adapter were:

- Ip address: 192.168.0.61
- Subnet Mask: 255.255.255.0
- Router: 192.168.86.1

The Mirage Node can then be connected to the USB to Ethernet adapter via an Ethernet cable. The Mirage node then needs to be configured with an IP address that is compatible with the network.

- myIP: 192.168.0.62 (different from adapter but within subnet)
- myGW: 192.168.86.1 (same as router)
- mySN: 255.255.255.0 (same as adapter)
- myDNS: 8.8.8.8 (Google DNS)

TODO(jcm-art): Implement a provisioning strategy for the Ethernet platform to enable different IP addresses to be automatically configured.


### Libraries

The Ethernet platform uses the `WebServer_WT32_ETH01` library, which is a fork of the `WebServer` library that adds support for the ESP32 Ethernet module. This library can be found at
https://github.com/khoih-prog/WebServer_WT32_ETH01.

### Provisioning

Note that provision, IOO must be pulled to ground to signal that it is waiting for firmware; after this is set to ground, reboot the board. After the firmware is loaded, remove this and reboot again.


## WLED Platform

### 


WLED Address: 192.168.87.62