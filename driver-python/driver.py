## 
# @file driver.py
# @author Jack Duignan (JackpDuignan@gmail.com)
# @date 2025-01-02
# @brief Hardware interface driver for the radio and msfs-2020

import threading
import serial
import time

from serial_packet_handler import SerialPacketHandler

def read_from_serial(ser: serial.Serial):
    """Thread function to continuously read from the serial port."""
    while True:
        try:
            # Read available data
            if ser.in_waiting > 0:
                response = ser.read_until(b"\n")
                print(f"{time.time()} Received: {response}")
        except Exception as e:
            print(f"Error while reading: {e}")
            break

def main():
    serialSender = SerialPacketHandler("/dev/ttyUSB0", 115200)

    # try:
    #     serialSender.serial_port.open()
    # except Exception as e:
    #         print(f"Error while opening serial port: {e}")
    #         return
    
    # Start the reading thread
    reader_thread = threading.Thread(target=read_from_serial, args=(serialSender.serial_port,), daemon=True)
    reader_thread.start()

    while True:
        time.sleep(5)
        serialSender.send_packet(b"123240560560", 0x00)


if __name__ == "__main__":
    main()