# Labs Completed for FreeRTOS Coursework

## Overview

This repository contains all of the labs that I completed as a part of my class in FreeRTOS. Please keep in mind that these were my first encounter with many of the concepts introduced in each lab and most of the code was provided to me. I have tried to, in post, add comments detailing which sections I wrote, denoted by `SECTION COMPLETED BY STUDENT`.

## Tools

- Each lab was completed using **ARM Keil v5**. The lab were originally created in ARM Keil v4; there are some weird issues that arose from this. Your mileage may vary.
- Every lab was developed and tested on the **TI Launchpad EK-TM4C123GXL** evaluation kit. This kit uses a **TM4C123GH6PM** microprocessor.
- Some labs involved using an LCD, model **ST7735**.

## Lab Outlines

### Lab 00a

This lab describes a program to handle seat assignments for a fictional airline. This fictional airline has **1** type of plane, each with a seating capacity of **12**. The seating configuration is **1-2** per row, for a total of **4** rows. This airline operates **1** flight daily. 

The program uses an array of 12 structs that contain:

- Seat ID
- Whether or not the seat is assigned
- Last Name
- First Name

The program displays a menu to a serial communication interface (UART). All communication between the user and the program is through such an interface. The menu items are:

- (a) Show the number of empty seats
- (b) Show a list of empty seats using seat numbers with letters
- (c) Show alphabetical list of seats
- (d) Assign a customer to a seat
- (e) Delete a seat assignment
- (f) Change the customer's seat
- (q) Quit

This lab contains the following embedded systems concepts:

- UART
- Embedded C

### Lab 00b

This lab is simply a Command Line Interface (CLI). This includes a parser for commands entered over serial communication (UART). The requirements are:

- Store a raw command string entered by the user, with a maximum length of 50 characters.
- Use an array-based structure to store processed tokens (up to 5 entries).
- Allow for easy customization of stored commands and tokens through parameterization.
- Default Parameters: If a valid command provides fewer than the expected number of arguments, assign a default value for the missing arguments.
- Hexadecimal Input Support: Accept hexadecimal numbers (prefixed by 0x).
- Command Alias Mapping: Support numeric RGB values and named colors.
- Live UART Command Entry: Accept commands via UART and implement a reliable method to detect command termination.

This lab contains the following embedded systems concepts:

- UART
- Embedded C
- String Parsing

### Lab 00c

This program continues the CLI. The main purpose of this section is to compare the scores of two different teams, determine the winner, and store all such related data. This required parsing score data, so that concept is expanded upon in this lab. There were a lot of requirements on how every edge case was handled, which I will not be repeating here. The logic was tested using a golden vector approach, where a test set of data was put through the logic of the program and the result checked by comparison to an expected result.

This lab contains the following embedded systems concepts:

- UART
- Embedded C
- String Parsing

### Lab 01a

This lab simply shows several ways to blink an LED on bare-metal embedded systems. Though it was not a requirement, I used the CLI to decide which case to check. The cases were as follows:

1. SysTick Interrupt
2. Timer Interrupt
3. PWM

This lab contains the following embedded systems concepts:

- UART
- Embedded C
- Interrupts
- SysTick
- GPIO
- Timers
- PWM

### Lab 01b

This lab has three major functionality requirements:

1. Use a timer to measure execution time.
2. Use two buttons to change the flashing frequency of two LEDs.
3. Change the UART implementation to be interrupt-based.

This lab contains the following embedded systems concepts:

- Interrupt-Based UART
- Embedded C
- Execution Time Measurement
- GPIO
- Floating Point Unit

### Lab 01c

This lab has three major functionality requirements:

1. ADC Sampling
2. Character Display to ST7735 LCD
3. Image Display to ST7735 LCD

This lab contains the following embedded systems concepts:

- ADC
- Display Output

### Lab 01d

This lab has the following functionality requirements:

1. **ADC Sampling**: The system should sample the voltages from `AIN0` to `AIN2` with sampling sequencer 3 at a rate of 1 MHz.
2. **UART CLI**: Entering a command "ADC" followed by a number will return the result from that channel.
3. **Display ADC Output**: The ST7735 should continuously display the average of the most recent 16 ADC samples for each channel.
4. **GPIO Configuration**: Configure an LED to blink.

This lab contains the following embedded systems concepts:

- ADC Sampling
- UART Communication
- Interrupts
- GPIO
- Display Output

### Lab 02a

This lab finally implements FreeRTOS concepts! In this, two tasks are defined: one to control the LEDs and one to detect button presses and notify the LED task. This is done through a Queue primitive.

This lab contains the following embedded systems concepts:

- FreeRTOS
- Tasks
- Queues
- GPIO

### Lab 02b

This lab continues the previous, adding the following functionality:

1. Creating a Mutex to ensure safe access of a shared resource (UART)
2. Create a task that handles UART communication

This lab contains the following embedded systems concepts:

- FreeRTOS
- Tasks
- Queues
- Semaphore
- Mutex
- UART

### Lab 02c

This lab implements periodic ADC sampling, uses the previous lab's UART task for UART communication, and implements an Event-based Control System. It contains the following embedded systems concepts:

- FreeRTOS
- Tasks
- Queues
- Mutex
- Event Bits
- UART

### Lab 02d

This lab is the capstone of the course, implementing nearly every concept covered in the labs. All of these tasks are implemented and then displayed out onto an LCD, which displays a "scoreboard" of all of the tasks, listing attributes such as state, priority, execution time, count, and memory usage. The following concepts are covered:

- FreeRTOS
- Embedded C
- Tasks
- Queues
- Mutex
- Timers
- Memory Management
- Display Output
- Resource Monitoring