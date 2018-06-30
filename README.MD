# Frocal

## Files in this repository

[Website-Farmer](./Website-Farmer) ([ link ](https://davidebecker.github.io/Frocal/Website-Farmer/)) - The interface where the farmer can view all data in detail (In adition to an ambient display)

[Website-Exhibition](./Website-Exhibition) ([ link ](https://davidebecker.github.io/Frocal/Website-Exhibition/)) - A demo site that showcases most of the functionality and guides a user though the concept.

[Arduino-Sketches/Master](./Arduino-Sketches/Master) - The Arduino Sketch for the Arduino that is located inside the store and collects data.

[Arduino-Sketches/Slave](./Arduino-Sketches/Slave) - The Arduino Sketch for the Arduino that recieves data from the store and allows a farmer to access the data. Also controls the ambient display.

# PINOUT

Both Arduinos are assumed to be Arduino UNOs

## Arduino #1 (Master)

Upload the /Arduino-Sketches/Master sketch to the Arduino

| Chip | PIN (Arduino) | PIN (Chip) |
| --- | --- | --- |

## Arduino #2 (Slave)

Upload the /Arduino-Sketches/Slave sketch to the Arduino

| Chip | PIN (Arduino) | PIN (Chip) |
| --- | --- | --- |