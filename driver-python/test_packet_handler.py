## 
# @file test_packet_handler.py
# @author Jack Duignan (JackpDuignan@gmail.com)
# @date 2025-01-02
# @brief Test the packet handler module

import unittest
from packet_handler import PacketHandler

class TestPacketHandler(unittest.TestCase):

    def test_packet_validate_empty_returns_fail(self):
        test_packet = b""
        result = PacketHandler.validate_packet(test_packet)
        self.assertEqual(PacketHandler.PacketStatus.SCHEMA_ERROR, result)

    def test_packet_validate_start_end_returns_fail(self):
        test_packet = bytearray(3)
        test_packet[0] = PacketHandler.PACKET_START_BYTE
        test_packet[1] = PacketHandler.PACKET_END_BYTE
        result = PacketHandler.validate_packet(test_packet)
        self.assertEqual(PacketHandler.PacketStatus.SCHEMA_ERROR, result)

    def test_packet_validate_start_end_type_crc_returns_fail(self):
        test_packet = bytearray(6)
        test_packet[0] = PacketHandler.PACKET_START_BYTE
        test_packet[PacketHandler.PACKET_IDENTIFIER_LOC] = 0x01  # Example identifier
        test_packet[2] = 0x10
        test_packet[3] = 0x12
        test_packet[4] = PacketHandler.PACKET_END_BYTE
        result = PacketHandler.validate_packet(test_packet)
        self.assertEqual(PacketHandler.PacketStatus.LENGTH_ERROR, result)

    def test_packet_validate_empty_packet_returns_valid(self):
        test_packet = bytearray(7)
        test_packet[0] = PacketHandler.PACKET_START_BYTE
        test_packet[PacketHandler.PACKET_IDENTIFIER_LOC] = 0x01  # Example identifier
        test_packet[PacketHandler.PACKET_LENGTH_LOC] = 0x00
        test_packet[3] = 0xFF
        test_packet[4] = 0xFF
        test_packet[5] = PacketHandler.PACKET_END_BYTE
        result = PacketHandler.validate_packet(test_packet)
        self.assertEqual(PacketHandler.PacketStatus.VALID, result)

    def test_packet_validate_full_packet_returns_valid(self):
        test_packet = bytearray(8)
        test_packet[0] = PacketHandler.PACKET_START_BYTE
        test_packet[PacketHandler.PACKET_IDENTIFIER_LOC] = 0x01  # Example identifier
        test_packet[PacketHandler.PACKET_LENGTH_LOC] = 0x01
        test_packet[3] = 0x01
        test_packet[4] = 0xF1
        test_packet[5] = 0xD1
        test_packet[6] = PacketHandler.PACKET_END_BYTE
        result = PacketHandler.validate_packet(test_packet)
        self.assertEqual(PacketHandler.PacketStatus.VALID, result)

    def test_packet_validate_full_packet_bad_crc_returns_fail(self):
        test_packet = bytearray(8)
        test_packet[0] = PacketHandler.PACKET_START_BYTE
        test_packet[PacketHandler.PACKET_IDENTIFIER_LOC] = 0x01  # Example identifier
        test_packet[PacketHandler.PACKET_LENGTH_LOC] = 0x01
        test_packet[3] = 0x01
        test_packet[4] = 0x00
        test_packet[5] = 0x00
        test_packet[6] = PacketHandler.PACKET_END_BYTE
        result = PacketHandler.validate_packet(test_packet)
        self.assertEqual(PacketHandler.PacketStatus.CRC_ERROR, result)

    def test_packet_compile_errors_null_pointer(self):
        result = PacketHandler.compile_packet(None, None, 0, 0x02)  # Example identifier
        self.assertEqual(PacketHandler.PacketStatus.UNKNOWN_ERROR, result)

    def test_packet_compile_builds_empty_packet(self):
        test_packet_buf = bytearray(6)
        test_payload_buf = bytes()
        expected_packet = bytes([PacketHandler.PACKET_START_BYTE, 0x02, 0x00, 0xFF, 0xFF, PacketHandler.PACKET_END_BYTE])
        result = PacketHandler.compile_packet(test_packet_buf, test_payload_buf, 0, 0x02)
        self.assertEqual(PacketHandler.PacketStatus.VALID, result)
        self.assertEqual(expected_packet, test_packet_buf)

    def test_packet_compile_builds_empty_packet_null_payload(self):
        test_packet_buf = bytearray(6)
        test_payload_buf = None
        expected_packet = bytes([PacketHandler.PACKET_START_BYTE, 0x02, 0x00, 0xFF, 0xFF, PacketHandler.PACKET_END_BYTE])
        result = PacketHandler.compile_packet(test_packet_buf, test_payload_buf, 0, 0x02)
        self.assertEqual(PacketHandler.PacketStatus.VALID, result)
        self.assertEqual(expected_packet, test_packet_buf)

    def test_packet_compile_builds_packet(self):
        test_packet_buf = bytearray(9)
        test_payload_buf = bytes([0x00, 0x01, 0x02])
        expected_packet = bytes([PacketHandler.PACKET_START_BYTE, 0x02, 0x03, 0x00, 0x01, 0x02, 0xDF, 0xEF, PacketHandler.PACKET_END_BYTE])
        result = PacketHandler.compile_packet(test_packet_buf, test_payload_buf, 3, 0x02)
        self.assertEqual(PacketHandler.PacketStatus.VALID, result)
        self.assertEqual(expected_packet, test_packet_buf)

    def test_packet_compile_builds_packet_null_payload_fails(self):
        test_packet_buf = bytearray(6)
        test_payload_buf = None
        result = PacketHandler.compile_packet(test_packet_buf, test_payload_buf, 5, 0x02)
        self.assertEqual(PacketHandler.PacketStatus.UNKNOWN_ERROR, result)

if __name__ == "__main__":
    unittest.main()
