# Led Cube 8x8x8 Control software

## Copyright notice
This project is heavily inspired on http://www.instructables.com/id/Led-Cube-8x8x8/.
Our version of the led cube is build around a Atmega32u4 Breakout Board from Adafruit (http://www.adafruit.com/products/296).

The reason for writing our own software is that we have a different CPU and use USB instead of RS232. Also, we want to have full control from a pc/mac and run the default 
program only if no USB is detected.

## Compiling 
Compiling on a Mac requires AVR Crosspack to be installed. To make sure those tools are used instead of normal gcc/as, please source the SOURCEME file which fixes 
the path. Also make sure that avr-gcc and avr-as are also available as gcc/as.

## Install firmware
avrdude -p m32u4 -P /dev/tty.usbmodem621 -c avr109 -U flash:w:main.hex
