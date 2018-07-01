# Frocal - Fresh and Local

Fresh and Local is a smart farm store. It works without any staff, gives the farmer useful analytics data and allows him to see the state of his store at a glance via an ambient display. This repo contains all the technical components of the prototype.

## Files in this repository

[Website-Exhibition](./Website-Exhibition) ([ link ](https://davidebecker.github.io/Frocal/Website-Exhibition/))    
A demo site that showcases most of the functionality and guides a user though the concept.

[Website-Farmer](./Website-Farmer) ([ link ](https://davidebecker.github.io/Frocal/Website-Farmer/))    
The interface where the farmer can view all data in detail (In adition to an ambient display)

[Arduino-Sketches/Master](./Arduino-Sketches/Master)    
Sketch for the Arduino that is located inside the store and collects data.

[Arduino-Sketches/Slave](./Arduino-Sketches/Slave)    
Sketch for the Arduino that recieves data from the store and allows a farmer to access the data. Also controls the ambient display.

[Documents](./Documents)    
Various references and useful info.

# Setup

## Hardware

In order to run this project you'll need the following hardware:

* 2x Arduino UNO
* 2x nRF24L01 Wireless transceiver ICs
* 1x HX711 Load Cell Amplifier
* 2x Flying Fish MH Sensor
* 1x RGB LED
* 1x Blue LED

## PINOUT

Both Arduinos are assumed to be Arduino UNOs

### Arduino #1 (Master)

Upload the /Arduino-Sketches/Master sketch to the Arduino

You'll need:

* 1x Arduino Uno
* 1x nRF24L01 Wireless transceiver IC
* 1x HX711 Load Cell Amplifier

| Chip | PIN (Arduino) | PIN (Chip) |
| --- | --- | --- |
| nRF24 | 1 | - |
| nRF24 | 2 | - |
| nRF24 | 3 | - |
| nRF24 | 4 | - |
| nRF24 | 5 | - |
| nRF24 | 6 | - |
| nRF24 | 7 | - |
| --- | --- | --- |
| HX711 | 1 | - |
| HX711 | 2 | - |
| HX711 | 3 | - |
| HX711 | 4 | - |
| --- | --- | --- |
| Flying Fish | VCC | 5V |
| Flying Fish | GND | GND |
| Flying Fish | OUT | - |

### Arduino #2 (Slave)

Upload the /Arduino-Sketches/Slave sketch to the Arduino

You'll need:

* 1x Arduino Uno
* 1x nRF24L01 Wireless transceiver IC
* 2x Flying Fish MH Sensor
* 1x RGB LED
* 1x Blue LED

| Chip | PIN (Arduino) | PIN (Chip) |
| --- | --- | --- |
| nRF24 | 1 | - |
| nRF24 | 2 | - |
| nRF24 | 3 | - |
| nRF24 | 4 | - |
| nRF24 | 5 | - |
| nRF24 | 6 | - |
| nRF24 | 7 | - |
| --- | --- | --- |
| Flying Fish | VCC | 5V |
| Flying Fish | GND | GND |
| Flying Fish | OUT | - |
| --- | --- | --- |
| RGB LED | Red Pin | - |
| RGB LED | GND | GND |
| RGB LED | Green Pin | - |
| RGB LED | Blue Pin | - |
| --- | --- | --- |
| Blue LED | GND | GND |
| Blue LED | VCC | - |