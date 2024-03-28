# 8ChocChip

8ChocChip is an emulator for the Chip8 software that I am working on to learn the basics around emulation. I decided to program it in C++ because I wanted to do more in it.

The graphics and only library used in this is SFML which handles the window, inputs and sounds.

## TODO

There are a couple of things left to do until I would say it works well enough
- [ ] Fix issue with score not increasing in the `BLINKY` game
- [ ] Cleaner/more optimised code
- [x] Windows Support
- [ ] MacOS Support

## Usage

To run 8ChocChip, run it though the command line, and it requires the path to a program file to emulate. Some basic ROMS can be found [here](https://github.com/ericgrandt/chip8-emulator/tree/master/roms).

## Building

I'll never tell ;)

### Requirements

To build this it requires C++, CMake and whatever SFML and SFML Audio requires on your platform

# Credits

Thanks to these two blogs that helped me through creating this emulator
[How to Create Your Very Own Chip-8 Emulator](https://www.freecodecamp.org/news/creating-your-very-own-chip-8-emulator/)
[Guide to making a CHIP-8 emulator ](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)

## Libraries

Currently, two libraries are being used
- [SFML](https://github.com/SFML/SFML) - UI
- [NativeFileDialog](https://github.com/mlabbe/nativefiledialog) - A fork of this is used for the CMakeLists files (https://github.com/AlvaroBarua/nativefiledialog/tree/CMakeLists)