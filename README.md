# LED Controller Firmware
This project contains the code to run the REV compatible LED Controller board.
The LED Controller board is a sister board that attaches to a Wemos D1 mini board sporting an ESP8266.
This main board should run the latest version of the firmata firmware, which can be acquired and configured manually [here](https://www.arduinolibraries.info/libraries/configurable-firmata), or auto-generated [here](http://firmatabuilder.com/).

## Hardware


### Requirements
This firmware is written to run on an ATTiny85 running at 8Mhz. The timing is set up with this frequency in mind.

## Serial data transfer between boards
In the past I made use of SoftwareSerial and the Arduino framework to accomplish a serial connection between the main board and the LED Controller board.
This was a very unstable connection, however, resulting in over 30% of the received messages being corrupted due to timing issues. To circumvent this, I have implemented a set of interrupts. Scrapping the Arduino and SoftwareSerial libraries should also reduce the program size by a fair bit, allowing me to add more LED lighting patterns.

The first interrupt will be executed as soon as a high signal is received on the receiving pin. Upon execution it will setup a timer that will buffer the incoming bits. This timer will wait the exact amount of time between the middle of the signals to ensure that the incoming bit is read correctly. The second interrupt will execute as soon at the buffer reaches 8 bits, or 1 byte, allowing me to use the value received as I see fit.

Note: the project currently contains a version of ShiftLCD that I ported to C to be able to test the serial connection for stability. It will be removed later since the project does not include a shift register or LCD screen.

## FastLED

