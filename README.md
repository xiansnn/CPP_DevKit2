# C++ DevKit
This repository proposes a User Interface alternative version for RPi Pico.
All de classes are developped within Rasspberry Pi Pico VSCode extension. 
they must rebuilt according to the used SDK version et pico platform. 

Up to now, only one display device is available. It is the OLED SSD1306. Most of The code was derived from [Harbys github repository](https://github.com/Harbys/pico-ssd1306) works and framebuffer API aws inspired from mycropython API we can find [here](https://docs.micropython.org/en/latest/library/framebuf.html#module-framebuf).

## Complete documentation (still in construction)
Access to the the project web page with external browser [here](https://xiansnn.github.io/CPP_DevKit/).

After opening this pages with your web browser, you will find doxygen documentation [here](docs/html/index.html)


## Motivation

I know that we can find lot of well defined and used UI interface e.g. Adafruit in Arduino environment, but the main motivation was first to have fun devoloping (quite) from scratch a user interface en C++.

The second motivation was to have a set of C++ classes that make development of Raspberry Pi Pico C++ application more friendly with a small UI with OLED display (the one I've got is the SSD1306 0.92").
Considering the time required to refresh the small OLED screen through the I2C link, I've consider that it will be worth thinking about something that refreshes parts of the screen only when data have been changed.
This is why I've started thinking about framebuffer and the way we can upload part of framebuffer to the display. I then developped C++ classes to handle SSD1306 and framebuffer.

## Background
During my professional period of activities, I've got knowledge about Model Based Engineering, Object Oriented Design, and Mathematical Abstractions in general. This is why I've based this development according to MVC design rules (Model View Control).

## Files naming
The files directories have been inspired by the Pico SDK.
Structures are named struct_xxx.
Classes created for testing or example purpose are named test_xxx.
Files created for test or example purpose are named t_xxxx/

## Files tree
At the root, we found directories:
-  docs : that contains doxygen files,
-  include : where all xxx.h files are located,
-  src : where all xxx.cpp files are located,
-  tests : where all test files are located.

### "utilities_xxx" directory
It contains a debug probe classe that can be useful to monitor what's going on the microcontroller thanks to level and pulse generated through some GPIO output.

### "hardware_xxx" directory
I'll place here C++ classes that abstract in some way the corresponding SDK functions related the HW peripheral of the Pico.

### "widget" directory
The abstract classes here are those related to widgets.

### "devices" directory
These are external components that I'll use.
The first one is "device_switch". This will be used to handle any kind of switches, either by IRQ or by cyclic sample. Deboucing is implemented. The device_switch is the good candidate to implement Controller part of MVC.
Derived from switch is the rotary encoder "device_KY0_40", a favorite device to built universal simple interface to choose menu, set value etc...
The first display is "device_SSD1306" as promised. MAy be others will come if I need them and I've time.

### "ui_core" directory
This where the abstract classes that provide UI facilities can be found, together with framebuffer classe that provides all code for drawing texts and graphs.

Together with a "test_ui_core" directory where a set of test program are developped : "test_controlled_value" as an exmaple of Model, "test_widget_on serial_monitor" as an exmaple of View, "device_KY_040" already mentionned above as example of Control and "test_manager" as example about how to manage several Model ans View with a single rotary encoder / switch as Control. 







