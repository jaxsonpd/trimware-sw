# Setup

This file explains how to setup the project to be run on both linux and wsl.

## Installation for Linux

Ensure the system is up to date:

```bash
sudo apt-get update
sudo apt-get upgrade -y
```

Then install avr and avr-dude:

```bash
sudo apt-get install gcc-avr binutils-avr avr-libc
sudo apt-get install avrdude
sudo apt-get install ruby cmake
```

## USB passthrough for WSL

Credits:
https://learn.microsoft.com/en-us/windows/wsl/connect-usb

If using wsl2 then usb can be passed through using the following commands.
First find all usb devices attached to the host by running:

```bash
usbipd list
```

in an admin ps prompt. Then bind and attach the device using:

```bash
usbipd bind --busid <busid>
usbipd attach --wsl --busid <busid>
```

To find all the attached devices on the linux machine run:

```bash
lsusb
```

A device can be detached using:

```bash
usbipd detach --busid <busid>
```

## Compilation for the atmega328p

Credits: https://www.hackster.io/milanistef/introduction-to-bare-metal-programming-in-arduino-uno-f3e2b4

Create the object file using avr-gcc:

```bash
avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o project.o project.c
```

Then create the executable:

```bash
avr-gcc -mmcu=atmega328p project.o -o project
```

Then convert to binary executable:

```bash
avr-objcopy -O ihex -R .eeprom project project.hex
```

Then flash to the micro controller:

```bash
avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 115200 -U flash:w:project.hex
```

Sometimes the device can be at other bus locations including ttyACM* the correct location can be found using:

```bash
ls -l /dev/ttyUSB* /dev/ttyACM*
```
