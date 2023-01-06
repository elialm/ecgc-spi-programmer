# SPI programmer

This PlatformIO project implements a programmer to program the cartridge boot ROM via SPI.
This project is part of the [ecgc](https://efacdev.nl/pages/project/?name=ecgc) project.

# Usage

The programmer works via a serial port.
Commands can be sent to the programmer to read from and write to the SPI bus.

Commands are defined under [/doc/cmd.txt](/doc/cmd.txt).
