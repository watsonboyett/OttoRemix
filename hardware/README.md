
# Hardware

## Overview

Control board for servos, displays, and sensors.


## Features

### MCU
* Raytac MDBT50 module, using Nordic nRF52840 MCU
* Custom board design (very similar to Adafruit Feather nRF52840 express)

### Power
* LiPo battery input (using JST-PH header)
* Boost converter for battery to get 5V supply
* USB-C charging (rate is configurabale, default is set to 200mA)
* Power/shut-down switch, battery can be charged while ON or OFF

### Servo Motors
* 6 servo motor connections (3 on left side, 3 on right side)
* Powered from 5V supply

### LED Matrix
* P/N: Adafruit Featherwing LED Matrix
* Uses I2C bus for communication (address: 0x70)
* Powered from 5V supply
* 4-pin header

### Touch Sensor 
* P/N: Azoteq TPS43-201A-S
* Uses I2C bus for communication (address: 0x74)
* Uses GPIO input for interrupt signal from sensor
* Powered from 3.3V supply
* 5-pin header

### IMU 
* P/N: Bosch BMX160
* Uses I2C bus for communication (address: 0x68)

### Ultrasonic Range Sensor
* P/N: 
* Uses 2 GPIO pins for trig and echo signals
* Powered from 3.3V supply
* 4-pin header

### Speaker
* P/N: 
* Uses PWM output from MCU to drive audio amplifier
* RC filter on
* Powered from 5V supply
* 2-pin header

### Buzzer
* P/N: 
* Uses PWM output from MCU
* Uses transistor to drive buzzer from 5V supply
* 2-pin header

### ON/OFF Switch
* Controls the 3.3V and 5V regulator enable circuitry 
* Not wired in-line with power supplies, so low power switched can be used
* Very low current draw when in shut-down (approx. 40 uA)
* 2 pin header


## Manufacturing Info

* outer dimensions: 56 mm x 49 mm (2.2" x 1.9")
* thickness: 1.00 mm (0.040")
* min. trace/space: 8 mil, 8 mil
* min. drill: 0.25 mm (10 mil)
* min. cutout radius: 0.4 mm (0.015")
* surface finish: ENIG
* soldermask: both sides, black
* silkscreen: both sides, white
* impedance control: none
* stackup: 4 layers


## Revision History

### v1A

* Initial design

### v1B

* Added 100 uF caps near servo power supplies
* Changed external board caps to 10 uF
* Swapped location of buzzer and power-off headers
