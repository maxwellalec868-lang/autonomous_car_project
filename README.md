# Peripheral Driver Development – STM32F334R8

## Project Overview
I am currently foucsed on developing custom peripheral drivers for my NUCLEO-F334R8 board.
These drivers will then be used in a automated car project that I will be working on after these drivers are complete.
I mainly wanted to create my own drivers to get a better understanding of how certain peripherals function as well as the hardware layout
of STM32 microcontrollers (and ARM Cortex Series of processors).

I have 2 custom peripheral drivers so far:
(1) GPIO
(2) PWM

Some simple examples are included in this project to test my drivers. Currently these tests include Currently these tests in this project includes:
(1) Dims an LED
(2) Controls a servo motor


## Board
- STM32 Nucleo-F334R8
- Clock: 8 MHz HSI (default)

## Key Files
- **Src/** – main application code  
- **drivers/** – custom peripheral drivers (GPIO, PWM, etc.)  
- **Startup/** – startup and linker scripts  
- **STM32F334R8TX_FLASH.ld** – linker script

## Build
Project developed in **STM32CubeIDE**.
Compile and flash directly using the IDE.

## Notes
- PWM frequency set via TIM2 channel 1 prescaler and ARR
- 
