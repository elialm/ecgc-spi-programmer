# SPI programmer

This project is part of the [ecgc](https://efacdev.nl/pages/project/?name=ecgc)
project.

This PlatformIO project implements a programmer to operate on the cartridge's
main bus for performing functions to aid the cartridge's development.
This programmer was initially made when I needed a solution for programming
the boot ROM quickly while developing the bootloader.
This is further explained in my [Slow deploy](https://github.com/elialm/ecgc-firmware/issues/2) issue.

# Usage

The programmer works via a serial port.
Commands can be sent to the programmer to read from and write to the SPI bus.
Commands are defined under [/doc/commands.md](/doc/commands.md).

I also developed scripts for automating the programmer operations.
These can be found in my [ecgc-util](https://github.com/elialm/ecgc-util)
repository.