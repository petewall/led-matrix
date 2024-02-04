# LED Matrix

This project is a 32x32 LED matrix

## BoM
Matrix
* 1024 white 3mm LEDs
* 16 MAX7219 LED Matrix drivers
* 16 10µF electrolytic capacitors
* 16 0.1µF ceramic capacitors

Power:
* Micro USB plug

Control:
* 2 buttons
* RTC?
* DHT?

* Wemos D1 Mini


## Software Design

1. Initialize LED drivers
1. Display Mode class
    1. Analog Clock
    1. Digital Clock
    1. Grafana Logo
    1. Column by column
    1. Row by row
    1. Snow
    1. Starfield
    1. Temperature and Humidity
1. Web interface
    1. Software buttons
    1. Current screen
    1. List of display modes
