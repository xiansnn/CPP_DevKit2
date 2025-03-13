# C++ DevKit Motivation
This repository proposes an abstraction layer above the Raspberry-pico SDK. This why its name is inspired from Pico-SDK : CPP_DevKit.

It is built with an Object Oriented Programming (OOP) approach. the proposed classes are those that I've needed with the displays and sensor modules I have.
All the classes are developped with Raspberry Pi Pico VSCode extension. 

A special focus has been made on the usage of graphic widget and User Interface (UI).
I know that we can find lot of good works and well-defined GUI e.g. Adafruit in Arduino environment, but the main motivation, here, was first to enjoy developing (quite) from scratch a user interface in OOP C++.

Another motivation was to have a set of C++ OOP classes that makes development of Raspberry Pi Pico C++ applications more friendly with a small UI with OLED display (the one I've got is the SSD1306 0.92" 128x64).
Considering the time required to refresh the small OLED screen through the I2C link, I've consider that it will be worth thinking about something that refreshes parts of the screen only when data have been changed. 
This is why I've started thinking about widget and the way we can upload those widget to a part of the display framebuffer.

Most of The code was derived from [Harbys github repository](https://github.com/Harbys/pico-ssd1306) works and widget framebuffer API was inspired from mycropython API we can find [here](https://docs.micropython.org/en/latest/library/framebuf.html#module-framebuf).


## Background
During my professional period of activities, I've got knowledge about Model Based Engineering, Object Oriented Design, and Mathematical Abstractions in general. This is why I've based this development according to MVC design rules (Model View Control).

## File trees
At the root, we found directories:
-  docs : that contains doxygen files,
-  external : contains the CMAke file that helps to FetchContent the source code and headers into an expernal project.
-  include : where all xxx.h files are located,
-  src : where all xxx.cpp files are located,
-  tests : where all test files are located. These files are also good usage examples.

The directories include, src, and tests contain the following sub-directories:
- device : dedicated to the small PCB module we usually find in DIY projects : e.g.  rotary encoder, switch with debouce, short press, long press, time-out features, display screen...
- hw : it an abstraction layers to the hardware peripheral provided by the RP204O MCU. I2C management and PWM are the first ones I've needed.
- sw : this where all pure software features are located : UI core mechanisms, graphical and text widget
- utilities : It contains a debug probe classe that can be useful to monitor what's going on the microcontroller thanks to level and pulse generated through some GPIO outputs.

# Files naming
The file directories have been inspired by the Pico SDK.
- source file are named "xxx.cpp"
- hearder files are names "xxx.h" . they are called be an #include "rep/xxx.h" where "rep" is either "device", "hw", "sw", or "utilities"
- libraries are named rep_xxx
- Structures are named struct_xxx.
- Classes created for testing (or example) purpose are named test_xxx.
- Files created for test or example purpose are named t_xxxx/

# USAGE

## Complete documentation
The documentation is permenently updated.
Access to the the project web page with external browser [here](https://xiansnn.github.io/CPP_DevKit2/).
After opening this pages with your web browser, you will find doxygen documentation [here](docs/html/index.html)

## HOWTO use these libraries in an external project
A CMake file "fetch_cpp_devkit.cmake" is provided. it set up include directories to the include directories of all provided libraries.
It mus be included in the top level CMakeLists.txt after the project() command.

## HOWTO use MVC
MVC has three parts:
- Model : where is the core of the application.
- View : supported by widgets, its mission is only to show the values, datas of the Model. Several widget can be attached to a Model, showing different data of a same data viewed by different way.
- Control : supported by any means (switch, rotary encoder, ...). It allow the User to change something in the Model.

The Model maintains a flag that indicates when something has changes and must be viewed. This is a signal that triggs the efective (re)drawing of the display. Otherwise nothing happens, savin time of processing.















