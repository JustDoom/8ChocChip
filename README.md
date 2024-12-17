# 8ChocChip

8ChocChip is an emulator for the Chip8 software that I am working on to learn the basics around emulation.

This uses SDL to handle graphics, input, audio and some other small things.
libconfig is used to manage config and save data files, and NativeFileDialog-extended is used for basic popup windows like the file/directory selector.

## TODO

There are a couple of things left to do until I would say it works well enough
- [x] Proper flag compatibility unlike many out there
- [ ] Cleaner/more optimised code
- [x] Windows Support
- [ ] MacOS Support (Builds, but unable to test)
- [x] Switch to SDL from SFML

## Usage

To run 8ChocChip you can either launch the executable file with the command line and specify a rom to instantly launch into like this `/path/to/exectable/8ChocChip --rom /path/to/rom.8ch`. 
Or you can normally launch the executable to open the UI and manage it from there.

Some basic ROMS can be found [here](https://github.com/ericgrandt/chip8-emulator/tree/master/roms).

More games are [here](https://github.com/dmatlack/chip8/tree/master/roms/games)

For a proper test suite you can check out https://github.com/Timendus/chip8-test-suite

## Building

Until a proper building section is created, just follow the commands that are used in the GitHub actions workflow for your platform.

https://github.com/JustDoom/8ChocChip/blob/master/.github/workflows/build.yml

Follow the commands but not everything is required to be executed exactly as the file does.
Some parts are just setting up ssh or installing dependencies which you may have installed already or have different methods to accomplish the task.

### Requirements

To build this it requires C++, CMake and whatever SDL/dependencies need. Check build file above.

# Credits

Thanks to these two blogs that helped me through creating this emulator. These are actually based on a "broken" emulator guide so not everything will work. But it is good as a base and learning how to do you own research into the functionality of the emulator.
- [How to Create Your Very Own Chip-8 Emulator](https://www.freecodecamp.org/news/creating-your-very-own-chip-8-emulator/)
- [Guide to making a CHIP-8 emulator ](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)

## Libraries

Currently, three libraries are being used
- [SDL](https://github.com/libsdl-org/SDL) - UI, graphics, input and sounds
- [NativeFileDialog-extended](https://github.com/btzy/nativefiledialog-extended) - Handles file dialogs for selecting files/directories. Fork of [nativefiledialog](https://github.com/mlabbe/nativefiledialog) which I used a fork of that added only CMake support, this new one adds that plus new fixes/features
- [libconfig](https://github.com/hyperrealm/libconfig) - Library to manage save data (like directories that hold ROMs) and possible future config files
