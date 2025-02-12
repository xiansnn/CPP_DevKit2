# C++ DevKit
This repository proposes a User Interface alternative version for RPi Pico.
All the classes are developped with Raspberry Pi Pico VSCode extension. 

Up to now, only one display device is available. It is the OLED SSD1306. Most of The code was derived from [Harbys github repository](https://github.com/Harbys/pico-ssd1306) works and framebuffer API was inspired from mycropython API we can find [here](https://docs.micropython.org/en/latest/library/framebuf.html#module-framebuf).

## Complete documentation
The documentation is permenently updated.

Access to the the project web page with external browser [here](https://xiansnn.github.io/CPP_DevKit2/).

After opening this pages with your web browser, you will find doxygen documentation [here](docs/html/index.html)


## Motivation

I know that we can find lot of good works and well-defined UI interface e.g. Adafruit in Arduino environment, but the main motivation, here, was first to enjoy developing (quite) from scratch a user interface in C++.

The second motivation was to have a set of C++ classes that makes development of Raspberry Pi Pico C++ applications more friendly with a small UI with OLED display (the one I've got is the SSD1306 0.92" 128x64).
Considering the time required to refresh the small OLED screen through the I2C link, I've consider that it will be worth thinking about something that refreshes parts of the screen only when data have been changed. 
This is why I've started thinking about framebuffer and the way we can upload part of framebuffer to the display. I then developped C++ classes to handle SSD1306 and framebuffer. This is what is proposed with the set of UI_core and widget classes.

## Background
During my professional period of activities, I've got knowledge about Model Based Engineering, Object Oriented Design, and Mathematical Abstractions in general. This is why I've based this development according to MVC design rules (Model View Control).

## Files naming
The files directories have been inspired by the Pico SDK.
Structures are named struct_xxx.
Classes created for testing (or example) purpose are named test_xxx.
Files created for test or example purpose are named t_xxxx/

## File trees
At the root, we found directories:
-  docs : that contains doxygen files,
-  include : where all xxx.h files are located,
-  src : where all xxx.cpp files are located,
-  tests : where all test files are located.

### src directories
This is where we can find all the source code for libraries, organized according to their categories:

#### "device_xxx" directories
These are external components or small module PCB that I'll use.
The most used is probably "device_switch_button". This will be used to handle any kind of switches, either by IRQ or by cyclic sample. Deboucing is implemented. The device_switch is the good candidate to implement Controller part of MVC.
Derived from switch is the rotary encoder "device_rotary_encoder", a favorite device to built universal simple interface to choose menu, set value etc... the related PCB module is the device_KY040 which is a rotary encoder, combined with a central switch button.
The first display is "device_SSD1306". May be others will come if I need them and I've time.

#### "hw_xxx" directories
I'll place here C++ classes that wrap, according to OOP, the corresponding SDK functions related the Hardware peripherals of the Pico.

#### "sw_xxx" directories
The abstract classes here are those related to software functions, such as:

#### "sw_framebuffer" directory
Framebuffer is the base for all text and graphic related functions.

#### "sw_ui_core" directory
This where the abstract classes, that provides UI facilities, can be found, together with framebuffer classes that provides all code for drawing texts and graphs.

Together with a "test_ui_core" directory where a set of test program are developped : "t_controlled_value" as an example of Model, "t_widget_on serial_monitor" as an exmaple of View, "device_KY040" already mentionned above as example of Control and "t_manager" as example about how to manage several Models and View with a single rotary encoder / switch as Control. 

#### "sw_widget" directory
This abstract class gives the main mechanisms to handle widget.

#### "sw_widget_xxx" directories
This where are all the widgets classes.

### "utilities_xxx" directories
It contains a debug probe classe that can be useful to monitor what's going on the microcontroller thanks to level and pulse generated through some GPIO output.












