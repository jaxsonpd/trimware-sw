## 
# @file serial_packet_handler.py
# @author Jack Duignan (JackpDuignan@gmail.com)
# @date 2025-01-02
# @brief Wrapper for a serial interface to the packet handler

import serial
from typing import Optional

from packet_handler import PacketHandler

class SerialPacketHandler:
    def __init__(self, port: str, baudrate: int = 9600, timeout: float = 1.0):
        self.serial_port = serial.Serial(port, baudrate, timeout=timeout)
        self.packet_handler = PacketHandler()

    def send_packet(self, payload: bytes, packet_identifier: int) -> bool:
        payload_length = len(payload)
        packet_length = (PacketHandler.HEADER_SIZE +
                         payload_length +
                         PacketHandler.CRC_LENGTH +
                         PacketHandler.FOOTER_SIZE)

        packet_buf = bytearray(packet_length)
        status = self.packet_handler.compile_packet(packet_buf, payload, payload_length, packet_identifier)

        print(f"Sending {packet_buf}")
        if status == PacketHandler.PacketStatus.VALID:
            for byte in packet_buf:
                self.serial_port.write(bytes([byte]))
            return True
        return False

    def receive_packet(self) -> Optional[bytes]:
        buffer = bytearray()

        while True:
            if self.serial_port.in_waiting:
                byte = self.serial_port.read(1)
                buffer.extend(byte)

                if len(buffer) >= PacketHandler.MIN_PACKET_LENGTH and buffer[-1] == PacketHandler.PACKET_END_BYTE:
                    status = self.packet_handler.validate_packet(buffer)
                    if status == PacketHandler.PacketStatus.VALID:
                        payload_start = PacketHandler.PACKET_PAYLOAD_START_LOC
                        payload_length = buffer[PacketHandler.PACKET_LENGTH_LOC]
                        return buffer[payload_start:payload_start + payload_length]
                    else:
                        return None

    def close(self):
        self.serial_port.close()
