Mirage Ethernet Network Configuration
===========

# Overview

This module contains insructions to complete the network configuraiton of Mirage, primarily with setting up the Mirage Cacti Nodes which are the ESP32-based microcontroller that controls the LED strips and communicates with the Mirage Controller.

# Network

## Usage Instructions

## Setup

To 

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


### Provisioning

Note that provision, IOO must be pulled to ground to signal that it is waiting for firmware; after this is set to ground, reboot the board. After the firmware is loaded, remove this and reboot again.


### WLED Configuration


TODO(jcm-art): Update WLED configuration instructions

WLED Address: 192.168.87.62