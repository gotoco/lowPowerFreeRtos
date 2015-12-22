Low Power sample project for STM32L15x series  (ready to go with STM32L152Discovery and GCC)
================

FreeRTOS port for STM32L152 uC with low power features

This repo shows basic project configuration and can be a start point for creating real world application for STM32L1xx based embedded systems.
It also contains high performed configuration for peripherals like I2C, SPI, Flash, USART and USB (so each possible peripherals).

Application is tested and developed at STM32L152Discovery board, but it can be used as well as on custom board with STM32L15x chip,
also, can use chips that are in /driver section

------------
Why has this repo?
------------
In recent years, I had a lot of projects, especially for low power consumption electronic devices. Every time when I started a new project I was trying to figure out past mistakes from past projects and how to omit them in a new project. Also, my goal was to don't make overengineering as well as follow KISS (keep it simple stupid) guidelines. 
As an effect, I created a repository for STM32L1xx uC, which I pull every time when I have to write a new project, rather than gathering whole code from scratch.
If you ever wrote embedded system based on small microcontroller you probably know that whole system is the same application, there is no separation between OS, LibC, drivers and Your code. In this repository I try to keep clean separation between these components, because I have see a lot of complication because of keeping different layers of software as a spaghetti. 
My intention is:
- has ready to go code that collects together: RTOS - FS - Drivers..(Your Drivers, Vendors Drivers, 3rd party Drivers)
- trying to create layers of insulation for the individual components
- has the possibility of development and evolution over time
- anyone can use and save time

------------
How to use?
------------
Application is dedicated to run wit open source tools (and fully tested) so If You have GCC, ARM-Toolchain, and OpenOCD You can easily build it and debug using in example embedded st-Link with STM32L_Discovery demo board or any other.

First step: download and install Toolchain for Cortex-M3 uC 
I strongly recommend Bare-metal toolchain for Cortex-R/M and Cortex-A it can be download at: 

     https://launchpad.net/gcc-arm-embedded/4.7/4.7-2013-q3-update

Second clone this repo:

     git clone https://github.com/skirki/lowPowerFreeRtos

Last thing:
come to project dir and type "make"

     $>: make

Because the project is simple one GNU makefile it will compile Your project and You are ready to go.

Next things to flash Your uC download Open-OCD and You can create and develop real world low power embedded systems!!

------------
Project structure and best practices?
------------

Currently, this demo project contains followed branches (applications):

     -master : empty "ready to go" project with all needed stuff 
     -Usb: easy sample configuration of USB end device (serial port) warning needed some hardware fixes on board! more details in branch README
     -tmp : sample smart-thermometer application that uses externals: RTC (MCP9800), Accelerometer (LIS35DE) and RTC (M41T56C64), and show data on discovery-onboard LCD

About codding standards:

     For details of embedded C development Please read CodingStandards at $project_root


Structure of project and dirs descriptions:

     /$project_root
       - /Application   <- Your application and code logic is here, nowhere else!
       - /Configuration <- bsp, config, sys-config main.c and so on (this part is platform dependent) if You want to port from discovery to another board You should only modify code in this folder
       - /Drivers     <- for dealing with external silicon Your drivers should go here
       - /FatFS       <- 3party software File system 
       - /FreeRTOS    <- 3party software Task scheduler in example FreeRTOS
       - /debug       <- debuger scripts (ex. open-ocd or production code)
       - /hdr         <- Shortcut for Definition of registers (for high performance and low memory cons)
       - /peripherals <- Your on Chip peripherials code should go here (I2C, USART, SPI, FLASH and so on)
       
