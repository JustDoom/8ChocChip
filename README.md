# 8ChocChip
[![Discord](https://img.shields.io/discord/810752039470235688?style=for-the-badge&logo=discord&label=Discord&labelColor=grey&color=green)](https://discord.imjustdoom.com)
[![X (formerly Twitter) Follow](https://img.shields.io/twitter/follow/ImJustDoom?style=for-the-badge&logo=x&label=Follow!&color=gray)](https://twitter.com/ImJustDoom)
[![Static Badge](https://img.shields.io/badge/Backers-%20?style=for-the-badge&logo=ko-fi&color=gray)](https://ko-fi.com/justdoom) [![GitHub](https://img.shields.io/github/stars/JustDoom/8ChocChip?style=for-the-badge&logo=github)](https://github.com/JustDoom/8ChocChip)

8ChocChip is a [Chip8](https://en.wikipedia.org/wiki/CHIP-8) emulator. It was created to help me learn and understand how emulators work.
I plan to support other Chip8 variants in the future with many other features like save states, per program configs, etc.

## Features

There are a couple of things left to do until I would say it works well enough
- [x] Proper flag/feature compatibility unlike many out there
- [ ] Cleaner/more optimised code
- [x] Windows Support
- [ ] MacOS Support
- [x] Switch to SDL from SFML
- [ ] Save state
- [ ] Configurable keys
- [ ] Database for auto setting selector
- [ ] Other Chip8 variants
- [ ] Per program settings (Keybindings, graphics, sounds, etc)
- [ ] Discord Rich Presence support
- [ ] JIT mode
- [ ] Proper install method

### Supported Platforms

- [x] GNU/Linux - Should work on any distro that will let you run plain executable files from the last few years at least
- [x] Windows 7 - Make sure to install [Visual Studio 2017](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170#visual-studio-2015-2017-2019-and-2022)
- [x] Windows 8(.1), 10, 11
- [ ] Windows 95
- [ ] Windows 98
- [ ] Windows XP
- [ ] Windows Vista
- [ ] MacOS 9
- [ ] MacOS X
- [ ] MacOS

## Usage

Just launch the downloaded executable and you are good to go. Some programs/games can be found [here](https://github.com/dmatlack/chip8/tree/master/roms/games).

For a proper test suite you can check out https://github.com/Timendus/chip8-test-suite

### Commands

`--speedtest` - Enabled speedtest mode. Can slightly increase performance by disabling some features that stop it from running at full speed.

`--rom <path to rom>` - Immediately launches into the specified ROM instead of the menu.

`--instructions <instruction count>` - The amount of instructions to run per frame (60fps).

`--debug` - Enabled printing extra debug information to the console

## Building

### Debian/Ubuntu

Install dependencies
```
sudo apt-get update && sudo apt-get install -y cmake build-essential g++ libsfml-dev libudev-dev libopenal-dev libvorbis-dev libflac-dev libxrandr-dev libxcursor-dev libgtk-3-dev
```

Clone the repository through the command line, or another option you may have
```
git clone https://github.com/JustDoom/8ChocChip.git
```

Download and project dependencies
```
git submodule update --init --recursive
```

Configure CMake
```
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

Build
```
cmake --build . --config Release
```

The generated files will be inside the `build/bin` directory

### Windows

I don't have a Windows machine set up to test so for now just follow the [workflow](https://github.com/JustDoom/8ChocChip/blob/master/.github/workflows/build.yml#L68)

# Credits

Thanks to these two blogs that helped me through creating this emulator.
- [Timendus Chip8 Test Suite](https://github.com/Timendus/chip8-test-suite) - An incredible test suite for Chip8
- [Guide to making a CHIP-8 emulator ](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) - This one is a great resource on creating a proper emulator
- [Emulator Development Discord](https://discord.gg/dkmJAes) - A really nice discord about emulator development

## Libraries

- [SDL](https://github.com/libsdl-org/SDL) - UI, graphics, input and sounds
- [SDL-ttf](https://github.com/libsdl-org/SDL_ttf) - Text fonts
- [nlohmann/json](https://github.com/nlohmann/json) - Library to manage save data (like directories that hold ROMs) and possible future config files
