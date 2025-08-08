## About
This project is a fork of the original Nestopia source code, plus the 
Linux port. The purpose of the project is to make sure people who want
a standalone GUI for the Nestopia emulator have this option available.

Project Goals:
* Maintain a functional GUI release of the Nestopia emulator on major desktop platforms

Contributing/Issues:
* Contributions will be reviewed for anything related to the standalone GUI builds
* Issues related to core emulation will be closed. This project no longer maintains the core emulator. Please submit issues about core emulation upstream at https://gitlab.com/jgemu/nestopia
* When not using a tagged release, please understand that the code is volatile and nothing is set in stone.

The following platforms are supported:
* Linux, BSD, Windows

This project depends on the following libraries:
FLTK 1.3 (1.4 preferred), SDL2, libarchive, libepoxy, libsamplerate, zlib

## Installing Dependencies
Install dependencies required for building on Debian-based Linux distributions:
```
apt-get install build-essential autoconf autoconf-archive automake autotools-dev libarchive-dev libepoxy-dev libfltk1.3-dev libsamplerate0-dev libsdl2-dev zlib1g-dev
```

## FLTK Build
To build using Autotools (optional arguments in square brackets):
```
autoreconf -vif
./configure [--enable-doc]
make
```
Optionally:
```
make install
```

### macOS Build
```
# Install dependencies
brew install autoconf automake autoconf-archive pkg-config libarchive libepoxy libsamplerate fltk sdl2

# Build
autoreconf -vif

# Set pkg-config to find Homebrew-installed libraries (works on both Intel and Apple Silicon Macs)
export PKG_CONFIG_PATH="$(brew --prefix)/lib/pkgconfig:$(brew --prefix libarchive)/lib/pkgconfig:$PKG_CONFIG_PATH"

c[--enable-doc]
make [install]
```

## Win32 Build
To build the win32 solution with Visual Studio 2010:
1. Ensure you have the DirectX 9 SDK
2. Manually zip NstDatabase.xml to the destination source/core/database/NstDatabase.zip
3. Open projects/nestopia.sln
4. Build in release mode

## Timer Info
The built in timer can be customized through timer.txt  
This is done with sets of conditions, and if none of the conditions are false, it sets the timer to the indicated state  
The included timer.txt has example conditions for Super Mario Bros.   
Note the emulator must be closed and reopened for changes in timer.txt to take effect

### Condition Types
start - sets the timer to start running  
stop - stops the timer from running. It may be started again if a start condition is met  
reset - resets the timer to the start frame (0 by default) and stops it  
end - stops the timer from running. It cannot be started again without resetting  
vpause - visually pauses the timer for some number of frames (60 by default). After, it will continue to run as if it hadn't been stopped  

### Conditions
Each condition is an address, a comparison operator, and a value  
Possible comparison operators are = or ==, <, >, <=, >=, and !=  

### Syntax 
Whitespace isn't needed and is ignored  
Numbers can be prefaced with 0x to signify hex  
//This is a comment

The syntax for lists of conditions is
```
<type> { <condition>* }
<condition> => <number> <operator> <number>
```
### Timer Settings
```delay <number>``` can be used to set a negative starting point  
```advance <number>``` can be used for a positive starting point  
```pausetime <number>``` changes the number of frames vpause stops the timer for

### Variables 
Numbers can be assiged to variables with ```let <var name> = <number>```   
Variables can be used in place of numbers in conditions or settings  
Variables must be alphanumberic and cannot start with a digit
