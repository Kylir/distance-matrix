# distance-matrix

A distance sensor, a 8x8 LED matrix and a microcontroller walk into a bar...

## Description

This is a silly little project but there is educational value in it. (At least it was very educational for me!)

The project uses an 8x8 LED matrix to display a kind of chart of the distance over the time.

You move your hand in front of the distance sensor. the closer you are the more dots will be switched ON on the screen.

+ An object at a distance of 40cm or more will not switch any LED.
+ An object at a distance of 5cm or less will switch all the LEDs.

## How to build

Assumption is you have an AVR programmer and avr-gcc, avrdude, etc. installed and working.
You need to tweak the `Makefile` for your flavor of microcontroller and programmer.

Then you can run `make flash` and it should build and flash the microcontroller.

## Credits

The code to deal with the LED matrix is inspired by the [Adafruit library available here](https://github.com/adafruit/Adafruit_LED_Backpack).

The code to get a distance measurement from the sensor is taken from [this GitHub repo here](https://github.com/yetifrisstlama/vl53l0x-non-arduino).

The I2C library used is [the brilliant I2CMaster found here](http://homepage.hispeed.ch/peterfleury/avr-software.html).


