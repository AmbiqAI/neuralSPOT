# Wiring up an Arducam

The ns-camera component (currently) only supports SPI-base Arducam Mega 5MP (maybe 3MP too, but it hasn't been tested). To connect the Arducam to an Apollo4 EVB, make the following connections:

1. Camera GND to any EVB GND
2. Camera 5V/VDD to any EVB 5V pin
3. Camera SCK to EVB Pin 9
4. Camera MISO to EVB Pin 10
5. Camera MOSI to EVB 9
6. Camera CS to EVB 11
