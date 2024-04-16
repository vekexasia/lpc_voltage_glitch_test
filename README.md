# Voltage glitcher test for LPC1768

Project Structure:

 - lpc/: contains all lpc codebase needed for the test
 - glitcher/: contains all pi2040 code for glitching


## Glitcher

The glitcher controller is pi2040. It uses the PIO interface to drive a single pin high or low at clock speed.

## LPC firmware

The lpc firmware does boot, put P0[26] LOW and start the target loop. Once finished P0[26] is raised HIGH again, and after a delay P0[26] is set to low again just before the target loop is executed again and so on.




## Stuff needed


- An LPC1768 board: **the target**
- MAX4619 chip for the glitching: **glitcher**
- Raspberry PI PICO 2040: **controller**


## Wiring

| LPC | PI2040 | MAX4619         |
|---|---|-----------------|
| GND | GND | GND,NC,X0,Y0,Z0 |
|  | VCC_EN | VCC             |
|  | 18 | EN              |
|  | 19 | A,B,C           |
| VCC |  | X,Y,Z           |
|  | VCC | X1,Y1,Z1        |
| P0.26 | 11 |                 |


## Settings

glitcher/glitch.c contains the following lines:

```c
#define PATTERN (0b111100000000000)
#define PATTERN_REPEAT 5
#define PATTERN_LENGTH 15
```

The pattern regulates when the glitch to GND is going to happen (1).

As of now this is the most stable without BO setting I could get. (no glitches yet with BO enabled)

Brown Out Detection can be disabled by connecting P2[13] of LPC1768 to GND upon boot.



---- 

Install: 
```bash

sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
git submodule update --init --recursive
# add PICO_SDK_PATH to PATH
```

Download https://developer.arm.com/downloads/-/gnu-rm
```bash
sudo apt remove binutils-arm-none-eabi libnewlib-arm-none-eabi gcc-arm-none-eabi
sudo tar -xvf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar  -C /usr/share
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc 
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-size /usr/bin/arm-none-eabi-size
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-objcopy /usr/bin/arm-none-eabi-objcopy
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-objdump /usr/bin/arm-none-eabi-objdump


sudo apt install libncurses-dev
sudo ln -s /usr/lib/x86_64-linux-gnu/libncurses.so.6 /usr/lib/x86_64-linux-gnu/libncurses.so.5
sudo ln -s /usr/lib/x86_64-linux-gnu/libtinfo.so.6 /usr/lib/x86_64-linux-gnu/libtinfo.so.5
```

Check
```bash
arm-none-eabi-gcc --version
arm-none-eabi-g++ --version
arm-none-eabi-gdb --version
arm-none-eabi-size --version
```
Notes: taken from: https://askubuntu.com/questions/1243252/how-to-install-arm-none-eabi-gdb-on-ubuntu-20-04-lts-focal-fossa


The board used for this repo is this one: ![img.png](docs/img.png) and could be found in the docs folder.


