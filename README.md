
# Final Degree Project
## "Kronos" RISC-V Emulator

This repository contains the source code of "Kronos", which consists of a rv32i RISC-V ISA implementation


# Instalation
Install the "Qt Creator" and use it to compile the project. Also you can use CMake with the libraries installed to compile it from source too.

## Documentation
In first place, this application features a top navigation menu with the following options:
- Archivo: Allows uploading a program, a campaign or close the application.
- Generar: Allows generating a campaign for an specific program.

The emulator has several panels to perform different actions. Additionally, there are 4 buttons on top of the terminal (black canvas):
- Stop button: Stops the execution and resets the emulator.
- Pause button: Stops the execution without resetting the emulator.
- Execute button: Executes the program loaded in memory.
- Step-by-step execution button: Execute 1 clock cycle.

In addition, below the RAM canvas, there is a search box that allows you to look up to a specific memory location. The search format is in hexadecimal without specifying "0x":
- Correct: 80003020
- Incorrect: 0x80003020

# Dependencies
As this emulator supports code editting with syntax highlight of C and Assembly, you must have installed CLang in order to compile code directly. 

## Install CLang on Windows
Go to [LLVM Official Webpage](https://llvm.org/builds/) and download from there the windows installer.

Make sure you have risc-v available as a target
```
clang --print-targets | Select-String risc
```
You should have 2: riscv32 and riscv64. This emulator currently supports riscv32.

## Author
- [@ikeruco29](https://www.github.com/ikeruco29)