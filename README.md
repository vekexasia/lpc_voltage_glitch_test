# Voltage glitcher test for LPC1768

Project Structure:

 - lpc/: contains all lpc codebase needed for the test
 - glitcher/: contains all pi2040 code for glitching


Wiring:

| LPC | PI2040 | MAX4619         |
|---|---|-----------------|
| GND | GND | GND,NC,X0,Y0,Z0 |
|  | VCC_EN | VCC             |
|  | 18 | EN              |
|  | 19 | A,B,C           |
| VCC |  | X,Y,Z           |
|  | VCC | X1,Y1,Z1        |
| P0.26 | 11 |                 |
