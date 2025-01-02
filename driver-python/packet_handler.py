## 
# @file packet_handler.py
# @author Jack Duignan (JackpDuignan@gmail.com)
# @date 2025-01-02
# @brief Classes to handle packet communication using the custom CAN inspired
# protocol

class PacketHandler:
    PACKET_CRC_POLYNOMIAL = 0x1021  # CRC-16-CCITT polynomial
    MIN_PACKET_LENGTH = 6  # Minimum valid packet length
    HEADER_SIZE = 3  # Start byte + command byte + length byte
    CRC_LENGTH = 2  # Length of CRC field
    FOOTER_SIZE = 1  # End byte

    PACKET_START_BYTE = 0x7E
    PACKET_END_BYTE = 0x7E

    PACKET_IDENTIFIER_LOC = 1
    PACKET_LENGTH_LOC = 2
    PACKET_PAYLOAD_START_LOC = 3

    class PacketStatus:
        VALID = "PACKET_VALID"
        SCHEMA_ERROR = "PACKET_SCHEMA_ERROR"
        LENGTH_ERROR = "PACKET_LENGTH_ERROR"
        CRC_ERROR = "PACKET_CRC_ERROR"
        UNKNOWN_ERROR = "PACKET_UNKNOWN_ERROR"

    @staticmethod
    def calculate_crc16(data: bytes) -> int:
        crc = 0xFFFF
        if (data is None or len(data) == 0):
            return crc
        
        for byte in data:
            crc ^= (byte << 8)
            for _ in range(8):
                if crc & 0x8000:
                    crc = (crc << 1) ^ PacketHandler.PACKET_CRC_POLYNOMIAL
                else:
                    crc <<= 1
                crc &= 0xFFFF  # Ensure CRC stays 16-bit
        return crc

    @staticmethod
    def validate_packet(packetBuffer: bytes) -> str:
        if len(packetBuffer) < PacketHandler.MIN_PACKET_LENGTH:
            return PacketHandler.PacketStatus.SCHEMA_ERROR

        currentByte = 0
        state = "START_BYTE"
        payloadLength = 0

        while currentByte < len(packetBuffer) and state != "PACKET_COMPLETE":
            byte = packetBuffer[currentByte]

            if state == "START_BYTE":
                if byte != PacketHandler.PACKET_START_BYTE or currentByte != 0:
                    return PacketHandler.PacketStatus.SCHEMA_ERROR
                currentByte += 1
                state = "CMD_BYTE"

            elif state == "CMD_BYTE":
                if byte in (PacketHandler.PACKET_START_BYTE, PacketHandler.PACKET_END_BYTE) or currentByte != 1:
                    return PacketHandler.PacketStatus.SCHEMA_ERROR
                currentByte += 1
                state = "PACKET_LENGTH_BYTE"

            elif state == "PACKET_LENGTH_BYTE":
                if currentByte != 2:
                    return PacketHandler.PacketStatus.SCHEMA_ERROR
                payloadLength = byte
                totalLength = PacketHandler.HEADER_SIZE + payloadLength + PacketHandler.CRC_LENGTH + PacketHandler.FOOTER_SIZE
                if totalLength > len(packetBuffer):
                    return PacketHandler.PacketStatus.LENGTH_ERROR
                currentByte += 1
                state = "PACKET_DATA_BYTES"

            elif state == "PACKET_DATA_BYTES":
                if any(packetBuffer[currentByte + i] in (PacketHandler.PACKET_START_BYTE, PacketHandler.PACKET_END_BYTE)
                       for i in range(payloadLength)):
                    return PacketHandler.PacketStatus.LENGTH_ERROR
                currentByte += payloadLength
                state = "CRC_BYTES"

            elif state == "CRC_BYTES":
                if currentByte + 2 > len(packetBuffer):
                    return PacketHandler.PacketStatus.SCHEMA_ERROR
                crc16 = PacketHandler.calculate_crc16(packetBuffer[PacketHandler.PACKET_PAYLOAD_START_LOC:
                                                                    PacketHandler.PACKET_PAYLOAD_START_LOC + payloadLength])
                receivedCrc16 = (packetBuffer[currentByte] << 8) | packetBuffer[currentByte + 1]
                if crc16 != receivedCrc16:
                    return PacketHandler.PacketStatus.CRC_ERROR
                currentByte += 2
                state = "END_BYTE"

            elif state == "END_BYTE":
                if byte != PacketHandler.PACKET_END_BYTE:
                    return PacketHandler.PacketStatus.SCHEMA_ERROR
                state = "PACKET_COMPLETE"
                currentByte += 1
            
            else:
                return PacketHandler.PacketStatus.SCHEMA_ERROR

        return PacketHandler.PacketStatus.VALID if state == "PACKET_COMPLETE" else PacketHandler.PacketStatus.SCHEMA_ERROR

    @staticmethod
    def compile_packet(packetBuf: bytearray, payloadBuf: bytes, payloadLength: int, packetIdent: int) -> str:
        if packetBuf is None or (payloadBuf is None and payloadLength > 0):
            return PacketHandler.PacketStatus.UNKNOWN_ERROR

        packetBuf[0] = PacketHandler.PACKET_START_BYTE
        packetBuf[PacketHandler.PACKET_IDENTIFIER_LOC] = packetIdent
        packetBuf[PacketHandler.PACKET_LENGTH_LOC] = payloadLength

        for i in range(payloadLength):
            packetBuf[PacketHandler.PACKET_PAYLOAD_START_LOC + i] = payloadBuf[i]

        crc16 = PacketHandler.calculate_crc16(payloadBuf)
        packetBuf[PacketHandler.PACKET_PAYLOAD_START_LOC + payloadLength] = (crc16 >> 8) & 0xFF
        packetBuf[PacketHandler.PACKET_PAYLOAD_START_LOC + payloadLength + 1] = crc16 & 0xFF
        packetBuf[PacketHandler.PACKET_PAYLOAD_START_LOC + payloadLength + 2] = PacketHandler.PACKET_END_BYTE

        return PacketHandler.PacketStatus.VALID