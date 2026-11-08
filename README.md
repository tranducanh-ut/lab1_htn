# lab1_htn

Embedded Systems - Lab 1: Working with LEDs on the BKIT ARM4 kit (STM32F407ZGT6) using Zephyr RTOS v4.4.0.

| Folder | Exercise |
| --- | --- |
| `lab1_ex1` | Asymmetric LED Blink (LED3: 1 s ON / 2 s OFF) |
| `lab1_ex2` | Coordinated LED3 and Output Y0 (shared variable) |
| `lab1_ex3` | Independent Blinking Patterns (LED3 + Y0/Y1 sequence) |
| `lab1_ex4` | One Thread Function - Many Lights (thread parameters) |
| `lab1_ex5` | Traffic Light System Simulation |

Build and flash (from `%USERPROFILE%\zephyrproject\applications\<folder>`):

```
west build -p always -b stm32f4_disco .
west flash
```
