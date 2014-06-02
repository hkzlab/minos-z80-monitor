MINOS
================


MINOS (stands for MINi OS) is a small ROM Monitor I wrote for the Ithaca Audio Z80 CPU card inside my S-100 computer.
The monitor is coded in C (SDCC compiler) and assembly and is very small (under 2K) to fit on a 2716 EPROM.

Supported functions:
* Port IN/OUT
* Memory READ/WRITE
* Jump execution to an address
* XModem data upload

Commands:
Xaaaa		- Loads a binary file to memory at address "aaaa". EG: X5000
Iaa			- INputs a byte from port address "aa". EG: I01
Oaa,dd		- OUTputs byte "dd" to port "aa". EG: O01,AA
Raaaa		- Reads a byte from memory address "aaaa". EG: R5000
Waaaa,dd	- Writes byte "dd" to memory address "aaaa". EG: W5000,04
Jaaaa		- Jumps execution to address "aaaa". EG: J3000

Fabio Battaglia - hkzlabnet@gmail.com
