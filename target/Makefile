## 
# @file Makefile
# @author Jack Duignan (JackpDuigna@gmail.com)
# #date 2024-06-11
# @brief the make file for the embedded-terminal project

# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# User params
USBDEVICE = /dev/ttyUSB0
TARGET = main

# Flags
CFLAGS = -Os -DF_CPU=16000000UL -mmcu=atmega328p -Wall -Wstrict-prototypes -Wextra
MCU = -mmcu=atmega328p

# Commands
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
DEL = rm
MKDIR_P = mkdir -p

# Locations
SRCDIR = ./src
BUILDDIR = ./build

# Use all c files in directory
SRCS := $(shell cd $(SRCDIR) && find * -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILDDIR)/%.o)

all: $(BUILDDIR)/$(TARGET).out

# Create out file
$(BUILDDIR)/$(TARGET).out: $(OBJS)
	$(CC) $(OBJS) $(MCU) -o $@

# Create object files
$(BUILDDIR)/%.c.o: $(SRCDIR)/%.c
	@$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ 

# Flash to the MCC
.PHONY: flash
flash: $(BUILDDIR)/$(TARGET).out
	$(OBJCOPY) -O ihex -R .eeprom $(BUILDDIR)/$(TARGET).out $(BUILDDIR)/$(TARGET).hex
	avrdude -F -V -c arduino -p ATMEGA328P -P $(USBDEVICE) -b 115200 -U flash:w:$(BUILDDIR)/$(TARGET).hex
	$(SIZE) $(BUILDDIR)/$(TARGET).hex

# Launch a serial prompt
.PHONY: serial
serial: 
	putty -serial $(USBDEVICE) -sercfg 8,'2',57600,'n' -fn "Monospace 13"

# Clean
.PHONY: clean
clean:
	$(DEL) -rf $(BUILDDIR)
