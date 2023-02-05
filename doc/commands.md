# Command documentation

This document provides the commands that the SPI programmer implements.
These commands are sent via UART and parsed by the programmer to perform 
certain functions.

## Formats

All formats described in this document are presented as regular expressions
conforming to the Python [re](https://docs.python.org/3/library/re.html#regular-expression-syntax) module.
There are some exceptions:
- Hexadecimal numbers are often used, so I've chosen to replace `[0-9A-F]`
with just `x` since this makes is more concise.
A hexadecimal byte is then represented as `xx`.
- The command format below is explained a bit differently to make it more concise.

All commands have the format `[C]a*\n`, where:
- `[C]` is a single character denoting the command.
This can be any valid command character mentioned later in this document.
- `a*` can be any number of additional arguments based on the command.
- All commands end with a `\n` to denote the end of the command.

## Command list

### C : Enable/disable debug core

**Format:**  
`C[ED]\n`

**Arguments:**  
- `[ED]` - Indicate whether to **E**nable or **D**isable the core.

**Description:**  
This command will instruct the programmer to (dis)assert the `DBG_ENABLE`
pin on the cartridge.
Asserting this pin with a logic high will activate the debug core and
allow all other commands to run.
This will also put the Gameboy in reset, freeing the cartridge's main bus
for use with the SPI debugger.
If the core is disabled, than all other commands will fail one way or another.

Additionally when enabling the core, the programmer will automatically set some
configurations. This includes setting the address to `0x0000` and disabling the auto increment feature.

**Response:**  
`ACK\n`

Indicates successful command completion.

**Possible errors:**  
- `EINV\n` - error in the command format
- `EFLUSH\n` - error during initial flushing of the debug core
- `EINCD\n` - error during disabling of the auto increment feature
- `EADDR\n` - error during setting the address to `0x0000`

### A : Set address

**Format:**  
`A(xxxx)\n`

**Arguments:**  
- `(xxxx)` - The address the debug core must be set to.

**Description:**  
This command sets the address of the debug core.
Subsequent reads from and writes to the debug core will operate on this address.

**Response:**  
`ACK\n`

Indicates successful command completion.

**Possible errors:**  
- `EINV\n` - error in the command format
- `EHAC\n` - error when sending the command for setting the high address bytes
- `EHAD\n` - error when sending the high address bytes
- `ELAC\n` - error when sending the command for setting the low address bytes
- `ELAD\n` - error when sending the low address bytes

### I : Enable/disable auto increment

**Format:**  
`I[ED]\n`

**Arguments:**  
- `[ED]` - Indicate whether to **E**nable or **D**isable auto increment.

**Description:** 
Enables or disables the auto increment feature on the debug core.
If the auto increment feature is enabled, the address will incremented after 
each byte operation during a read or write.

For example, if the address is set to `0x4000` and 4 bytes are read, the read 
operation will return the bytes at addresses `0x4000` to `0x4003`.
The next read or write will then operate on address `0x4004`.

If the same read operation is performed while this feature is disabled,
then a 4 byte read will just return the byte at `0x4000` 4 times.

**Response:**  
`ACK\n`

Indicates successful command completion.

**Possible errors:**  
- `EINV\n` - error in the command format
- `EAIC\n` - error when sending the enable/disable auto increment command

### R : Read

**Format:**  
`R(xx)\n`

**Arguments:**  
- `(xx)` - Amount of bytes to be read minus 1 (e.g. 0F reads 16 bytes)

**Description:**  
This command will read an amount of bytes given by the command argument
at the set address.

**Response:**  
`(D(xx){1,32}\n){1,8}`

The response will consist of the read data as data frames.
These frames start with the character `D` and are followed by a maximum
of 32 hexadecimal bytes.
As the maximum number of bytes read is 256 (the argument being `0xFF`),
a read response will return at maximum 8 full frames.

Receiving all expected frames will signify a successful command completion.

**Possible errors:**  
- `EINV\n` - error in the command format
- `ERBC\n` - error when sending the burst read command
- `ERBZ\n` - error when the byte after a burst read is not 0
- `ERC\n` - error when sending the read command
- `ERZ\n` - error when the byte after a read is not 0

### W : Write

**Format:**  
`W(xx)\n`

**Arguments:**  
- `(xx)` - Amount of bytes to be written minus 1 (e.g. 0F writes 16 bytes)

**Description:**  
This command will writes an amount of bytes given by the command argument
at the set address.
The data to be written will be sent later using the
[Data frame](#d--data-frame) command.

**Response:**  
`ACK\n`

Indicates successful command completion.

**Possible errors:**  
- `EINV\n` - error in the command format
- `EBUSY\n` - response when this command is sent when the previous write has not fully been done yet.

### D : Data frame

**Format:**  
`D(xx){1,32}\n`

**Arguments:**  
- `(xx){1,32}` - The data to be written to the cartridge

**Description:**  
Data frame to be written to the cartridge.
This data frame is sent one or more times after a [Write](#w--write) command.

**Response:**  
`ACK\n`

Indicates successful command completion.

**Possible errors:**  
- `EINV\n` - error in the command format
- `EWROVER\n` - response when more bytes to be written have been sent than expected
- `EWBC\n` - error when sending the burst write command
- `EWBD\n` - error when sending the burst write data
- `EWBO\n` - error when closing off the burst write
- `EWBC\n` - error when sending the write command
- `EWBD\n` - error when sending the write data