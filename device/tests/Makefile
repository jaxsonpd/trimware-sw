## 
# @file Makefile
# @author Jack Duignan (JackpDuigna@gmail.com)
# 2024-03-30
# @brief various functions to make the test project

CC = avr-gcc
CFLAGS = -Os -DF_CPU=16000000UL -mmcu=atmega328p -Wall -Wstrict-prototypes -Wextra
OBJCOPY = avr-objcopy
SIZE = avr-size
DEL = rm
SRCDIR = ../src
INCDIR = ../src/include

USBDEVICE = /dev/ttyUSB0



# Default Target
all: test.out

# Compile the object files
test.o: test.c $(INCDIR)/UART.h
	$(CC) -c $(CFLAGS) $< -o $@

command.o: $(SRCDIR)/command.c $(SRCDIR)/command.h $(INCDIR)/UART.h $(SRCDIR)/IO.h
	$(CC) -c $(CFLAGS) $< -o $@

help.o: $(SRCDIR)/help.c $(SRCDIR)/help.h $(INCDIR)/UART.h
	$(CC) -c $(CFLAGS) $< -o $@

clear.o: $(SRCDIR)/clear.c $(SRCDIR)/clear.h $(INCDIR)/UART.h
	$(CC) -c $(CFLAGS) $< -o $@

led.o: $(SRCDIR)/led.c $(SRCDIR)/led.h $(INCDIR)/UART.h
	$(CC) -c $(CFLAGS) $< -o $@

IO.o: $(SRCDIR)/IO.c $(SRCDIR)/IO.h $(SRCDIR)/utils.h
	$(CC) -c $(CFLAGS) $< -o $@

utils.o: $(SRCDIR)/utils.c $(SRCDIR)/utils.h
	$(CC) -c $(CFLAGS) $< -o $@

UART.o: $(INCDIR)/UART.c $(INCDIR)/UART.h
	$(CC) -c $(CFLAGS) $< -o $@

GPIO.o: $(INCDIR)/GPIO.c $(INCDIR)/GPIO.h
	$(CC) -c $(CFLAGS) $< -o $@

test.out: test.o UART.o
	$(CC) -mmcu=atmega328p $^ -o $@


# Clean the project
.PHONY: clean
clean:
	$(DEL) *.o *.out *.hex

# Flash to the MCC
.PHONY: flash
flash: test.out
	$(OBJCOPY) -O ihex -R .eeprom test.out test.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P $(USBDEVICE) -b 115200 -U flash:w:test.hex
	$(SIZE) test.hex


# Launch a serial prompt
.PHONY: serial
serial: 
	putty -serial $(USBDEVICE) -sercfg 8,'2',57600,'n' -fn "Monospace 13"
