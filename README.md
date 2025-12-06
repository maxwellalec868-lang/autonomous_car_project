# Autonomous Mini-Car Project – STM32F334R8

## Project Overview

I am developing an autonomous mini-car that will posess basic obstacle detection and reactive control by stopping before it hits an object.
This vehicle will be controlled via an STM32 Nucleo board which I am writing peripheral drivers for.
It will be operated through my PC or a remote control (need to decide) communicating to an ESP32 which will communicate to the STM32.
Finally, it will contain a camera that will allow me to control the vechicle without it being directly in my line of sight (LoS).
I will be using a pre-built chasis that contains a servo motor for steering and a motor for acceleration and braking. 
This overview will get more detailed as the project progresses.

#### Project Phases
The project will be broken down into 4 phases. I am currently in Phase 1 and about to move to phase 2.
<br>**Phase 1:** Research attributes and compoents to include in this project and develop steps while doing some basic driver development.
<br>**Phase 2:** Driver development and hardware configuration to enable control of car.
<br>**Phase 3:** Integrate sensors to enable auto stopping.
<br>**Phase 4:** Integrate camera to allow for control outside of LoS.


#### My motivation for doing this project includes:
1. Enhancing my understanding of microcontroller peripherals and the software that drives them
1. Improve my C programming skills in embedded development
1. Gain understanding of STM32 microcontrollers (and ARM-Cortex MCU's) as well as ESP32's working together in a heterogeneous system
1. Development of an autonomous system containing multiple sensors and MCUs
1. Hanlde potential issues that arrise regarding power and task scheduling

#### Current Peripheral Drivers:
I currently only have two peripheral drivers. I will continue to develop these two and additional ones as needed.
- GPIO
- PWM

#### Current Status of Code
I currently only have code to test the drivers functionallity and control of servo motors using my PWM driver. All this code is located in the test directory.

Currently these tests include:
1. dim_LED_PWM: Dims an LED using PWM signals
2. btn_turn_motor: Continuously turns a servo motor (Currently doesn't use a button but will soon)
3. toggle_LED_intterupt: Toggles an LED on and off using an interrupt.


## Board
- STM32 Nucleo-F334R8
- Clock: 8 MHz HSI (default)

## Key Files
- **Src/** – Main application code  
- **drivers/** – Custom peripheral drivers (GPIO, PWM, etc.)
- **tests/** – Code to test individual driver components or hardware components 
- **Startup/** – Startup and linker scripts  
- **STM32F334R8TX_FLASH.ld** – linker script

## Build
Project developed in **STM32CubeIDE**.
Compile and flash directly using the IDE.

