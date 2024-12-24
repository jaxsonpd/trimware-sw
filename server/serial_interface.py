## 
# @file serial_interface.py
# @author Jack Duignan (JackpDuignan@gmail.com)
# @date 2024-12-01
# @brief A file to handle the serial communications between the firmware and the
# driver

import serial

serial_port = serial.Serial

def init_serial():
    global serial_port
    serial_port = serial.Serial(port="COM3", baudrate=9600, bytesize=8, parity="N", stopbits=1)
    try:
        serial_port.open()
    except:
        print("port is already open")

def get_commands() -> list:
    global serial_port
    if (serial_port.in_waiting == 0):
        return []
    
    events = []

    while(serial_port.in_waiting):
        events.append(serial_port.read_until())

    return events

