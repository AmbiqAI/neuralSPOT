# Compiling CMSIS-DSP
The two makefile in this directory

Preparing to compile:
1. Create a CMSIS directory
1. Clone CMSIS-DSP and CMSIS_5 into that directory
2. Copy the Makefile example including in *this* directory to CMSIS-DSP/Source/Makefile

This example makefile includes support for both CortexM4 and CortexM55 and both GCC and Armclang.

```bash
make # M4 and GCC are default
rm -rf builddir # makefile doesn't have a 'make clean' so do it by hand
make TARGET=m55 # M55/GCC
rm -rf builddir
make TOOLCHAIN=arm # M4/armclang
rm -rf buildir
make TOOLCHAIN=arm TARGET=m55 # M55/armclang
```