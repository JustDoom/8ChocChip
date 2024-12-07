# 8ChocChip

8ChocChip is an emulator for the Chip8 software that I am working on to learn the basics around emulation. 
I decided to program this in C++ because I wanted to do more in it and improve my skills.

The graphics and only library used in this is SFML which handles the window, inputs and sounds. 
libconfig is used to manage config and save data files, and NativeFileDialog is used for basic popup windows like the file/directory selector.

## TODO

There are a couple of things left to do until I would say it works well enough
- [ ] Fix some of the instructions that result in most programs failing except for `BLINKY`
- [ ] Cleaner/more optimised code
- [x] Windows Support
- [ ] MacOS Support (Builds, but unable to test)
- [ ] Switch to SDL from SFML

## Usage

To run 8ChocChip you can either launch the executable file with the command line and specify a rom to instantly launch into like this `/path/to/exectable/8ChocChip --rom /path/to/rom.8ch`. 
Or you can normally launch the executable to open the UI and manage it from there.

Some basic ROMS can be found [here](https://github.com/ericgrandt/chip8-emulator/tree/master/roms).

## Building

Until a proper building section is created, just follow the commands that are used in the GitHub actions workflow for your platform.

https://github.com/JustDoom/8ChocChip/blob/master/.github/workflows/build.yml

Follow the commands but not everything is required to be executed exactly as the file does.
Some parts are just setting up ssh or installing dependencies which you may have installed already or have different methods to accomplish the task.

### Requirements

To build this it requires C++, CMake and whatever SFML and SFML Audio requires on your platform. Check build file above.

# Credits

Thanks to these two blogs that helped me through creating this emulator
[How to Create Your Very Own Chip-8 Emulator](https://www.freecodecamp.org/news/creating-your-very-own-chip-8-emulator/)
[Guide to making a CHIP-8 emulator ](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)

## Libraries

Currently, three libraries are being used
- [SFML](https://github.com/SFML/SFML) - UI, graphics, input and sounds
- [NativeFileDialog](https://github.com/mlabbe/nativefiledialog) - A fork of this is used for the CMakeLists files (https://github.com/AlvaroBarua/nativefiledialog/tree/CMakeLists)
- [libconfig](https://github.com/hyperrealm/libconfig) - Library to manage save data (like directories that hold ROMs) and possible future config files
