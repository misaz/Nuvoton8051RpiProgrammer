# nu8051 utility

This project contains utility for Raspberry Pi (and other Linux capable SBCs) that allows you to program Nuvoton 8051 serie microcontrollers attached to Raspberry Pi's GPIO. Utility does not depends on any other external part. At Release page it is avalaible precompiled as a single exacutable binary for `armhf` platform (standard 32bit Raspbian) and also for the `aarch64` which is used on more modern OSs like 64bit Ubuntu or Alpine Linux.

## Installation
On 32bit OS like Raspbian run the following commands:

```
wget https://github.com/misaz/nu8051/v1.0/nu8051_armhf.tar.gz
tar -xzf nu8051_armhf.tar.gz
sudo cp nu8051 /usr/bin
cd -
```

On 64bit OS like Alpine Linux run the following commands:

```
wget https://github.com/misaz/nu8051/v1.0/nu8051_aarch64.tar.gz
tar -xzf nu8051_aarch64.tar.gz
sudo cp nu8051 /usr/bin
cd -
```

## Pinout

Connect your Nuvoton to the Raspberry Pi using following connections. 

> :warning: You MUST disconnect any 5V source from your Nuvoton otherwise you will burn your Raspberry Pi. If you need to power your nuvoton with 5V while programming you must use voltage level converter on RST, ICPCLK and ICPDAT line and diconnect VCC line. Common GND must remain. Note that some pasive level converters do not support signals as fast as 1MHz which is used when programming device.

```
Nuvoton's    GND <------> GND   (pin #9) on Raspberry Pi
Nuvoton's    VCC <------> 3V    (pin #1) on Raspberry Pi
Nuvoton's    RST <------> GPIO2 (pin #3) on Raspberry Pi
Nuvoton's ICPCLK <------> GPIO3 (pin #5) on Raspberry Pi
Nuvoton's ICPDAT <------> GPIO4 (pin #7) on Raspberry Pi
```

## Usage

### Read device info

To detect that device is connected properly try reading some device informations. Following command reads CID of connected device. If the result is ff check your wiring is wrong and check that Nuvoton is powered.

```
nu8051 dinfo cid
```

### Program using BIN file.

If you have your firmare in binary form you can flash it using following command. `firmware.bin` is name of the binary file that will be flashed to the device.

```
nu8051 ld write firmware.bin
```

### Program using HEX file.

If you have your firmware in Intel HEX format you need to convert it to BIN first. This could be done using following commands that expectes that your HEX file is `firmware.hex`.

```
objcopy -I ihex firmware.hex -O binary firmware.bin
nu8051 ld write firmware.bin
```

### More examples

Utility support another commands. Use help command to print help.

```
nu8051 help
nu8051 ld help
nu8051 dinfo help
nu8051 cbytes help
```

## Disclaimer

Program is provided without any warranty. Use it on your own risk.

## Building from sources

You can build program yourself. To compile and install output binary to `/usr/bin` folder run following.

```
git clone https://github.com/misaz/nu8051
cd nu8051
make
make install
```

## Under the hoods

It is userspace wrapper for my [Nuvoton 8051 Serie MCU Programming library](https://github.com/misaz/Nuvoton8051ProgrammingLib) library. Library is generic and program uses platform specific implementation accesing GPIO features using `/sys/class/gpio` Linux virtual file system.

If you need to automate flashing chips you can write own program based on that library directly.
