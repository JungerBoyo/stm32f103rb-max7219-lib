# stm32f103rb-template-project
stm32f103rb template project with basic CMSIS libs setup using Unix Makefiles.

## IMPORTANT
Replace PREFIX environment variable in Makefile with the path to arm-none-eabi toolchain dir on your machine in order to use a template. You also need st-flash flashing tool to be able to load the binary to microprocessor. 

## Debugging
To debug follow the steps:
* run st-util
* run gdb
* (in gdb)target extended:4242
* (in gdb)load <path_to_exe>.elf



