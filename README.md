# Chip8 Emulator
This is my attempt at a Chip8 Emulator. It is written in C++ and uses SDL as a cross-platform window library.
![](https://github.com/MattGuy00/Chippy8/blob/main/Trip8.png)

## What is a Chip8 Emulator?
> CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker on his 1802 microprocessor. It was initially used on the COSMAC VIP and Telmac 1800, which were 8-bit microcomputers made in the mid-1970s.
<br>Credit: [Wikipedia](https://en.wikipedia.org/wiki/Chip-8)

## Usage
Dependencies: SDL3
```
git clone https://github.com/MattGuy00/Chippy8.git
cd Chippy8
cmake .
```

Running the emulator:
```
./Chippy8 path/to/rom
```

## Controls
| 1 | 2 | 3 | 4 |
|---|---|---|---|
| Q | W | E | R |
| A | S | D | F |
| Z | X | C | V |

## Roms
Many games and demos can be found here:
https://github.com/kripod/chip8-roms


## Testing
Classic IBM logo: <br>
https://github.com/loktar00/chip8/blob/master/roms/IBM%20Logo.ch8

Some basic tests. Doesn't cover all opcodes: <br>
https://github.com/corax89/chip8-test-rom/blob/master/test_opcode.ch8 <br>
https://github.com/daniel5151/AC8E/blob/master/roms/bc_test.ch8

Comprehensive test suite that covers most, if not all, opcodes: <br>
(I ran roms 1, 3, 4, 5, 6 located in bin/) <br>
https://github.com/Timendus/chip8-test-suite
