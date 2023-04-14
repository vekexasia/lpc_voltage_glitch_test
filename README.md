# Voltage glitcher test for LPC1768

Project Structure:

 - lpc/: contains all lpc codebase needed for the test
 - glitcher/: contains all pi2040 code for glitching

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

