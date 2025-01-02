/**
 * @file test_packet_handler.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-27
 * @brief Tests for the packet handling module
 */


#include <stdint.h>
#include <stdbool.h>

#include "unity.h"

#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL

#include "packet_handler.h"
#include "packet.h"

void setUp(void) {

}

void tearDown(void) {

}

// =========================== Tests ===========================
void test_packet_validate_empty_returns_fail(void) {
    char* testPacket = "";
    
    packetStatus_t result = packet_validate(testPacket, 0);

    TEST_ASSERT_EQUAL(PACKET_SCHEMA_ERROR, result);
}

void test_packet_validate_start_end_returns_fail(void) {
    char testPacket[3] = {0};

    testPacket[0] = PACKET_START_BYTE;
    testPacket[1] = PACKET_END_BYTE;

    packetStatus_t result = packet_validate(testPacket, 3);

    TEST_ASSERT_EQUAL(PACKET_SCHEMA_ERROR, result);
}

void test_packet_validate_start_end_type_crc_returns_fail(void) {
    char testPacket[6] = {0};

    testPacket[0] = PACKET_START_BYTE;
    testPacket[PACKET_IDENTIFIER_LOC] = PACKET_HEARTBEAT;
    testPacket[2] = 0x10;
    testPacket[3] = 0x12;
    testPacket[4] = PACKET_END_BYTE;

    packetStatus_t result = packet_validate(testPacket, 6);

    TEST_ASSERT_EQUAL(PACKET_LENGTH_ERROR, result);
}

void test_packet_validate_empty_packet_returns_valid(void) {
    char testPacket[7] = {0};

    testPacket[0] = PACKET_START_BYTE;
    testPacket[PACKET_IDENTIFIER_LOC] = PACKET_HEARTBEAT;
    testPacket[PACKET_LENGTH_LOC] = 0x00;
    testPacket[3] = 0xFF;
    testPacket[4] = 0xFF;    
    testPacket[5] = PACKET_END_BYTE;

    packetStatus_t result = packet_validate(testPacket, 7);

    TEST_ASSERT_EQUAL(PACKET_VALID, result);
}

void test_packet_validate_full_packet_returns_valid(void) {
    char testPacket[8] = {0};

    testPacket[0] = PACKET_START_BYTE;
    testPacket[PACKET_IDENTIFIER_LOC] = PACKET_HEARTBEAT;
    testPacket[PACKET_LENGTH_LOC] = 0x01;
    testPacket[3] = 0x01;
    testPacket[4] = 0xF1;
    testPacket[5] = 0xD1;    
    testPacket[6] = PACKET_END_BYTE;

    packetStatus_t result = packet_validate(testPacket, 7);

    TEST_ASSERT_EQUAL(PACKET_VALID, result);
}

void test_packet_validate_full_packet_bad_crc_returns_fail(void) {
    char testPacket[8] = {0};

    testPacket[0] = PACKET_START_BYTE;
    testPacket[PACKET_IDENTIFIER_LOC] = PACKET_HEARTBEAT;
    testPacket[PACKET_LENGTH_LOC] = 0x01;
    testPacket[3] = 0x01;
    testPacket[4] = 0x00;
    testPacket[5] = 0x00;    
    testPacket[6] = PACKET_END_BYTE;

    packetStatus_t result = packet_validate(testPacket, 7);

    TEST_ASSERT_EQUAL(PACKET_CRC_ERROR, result);
}

void test_packet_compile_errors_null_pointer(void) {
    char* testPacketBuf = NULL;
    char* testPayloadBuf = NULL;

    packetStatus_t result = packet_compile(testPacketBuf, testPayloadBuf, 0, PACKET_UPDATE_FREQ);

    TEST_ASSERT_EQUAL(PACKET_UNKOWN_ERROR, result);
}

void test_packet_compile_builds_empty_packet(void) {
    char testPacketBuf[6] = {0};
    char testPayloadBuf[] = {0};
    char expectedPacket[] = {PACKET_START_BYTE, PACKET_UPDATE_FREQ, 0x00, 0xFF, 0xFF, PACKET_END_BYTE};
    uint8_t packetLength = 0;
    
    packetStatus_t result = packet_compile(testPacketBuf, testPayloadBuf, packetLength, PACKET_UPDATE_FREQ);

    TEST_ASSERT_EQUAL(PACKET_VALID, result);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expectedPacket, testPacketBuf, 6);
}

void test_packet_compile_builds_empty_packet_null_payload(void) {
    char testPacketBuf[6] = {0};
    char* testPayloadBuf = NULL;
    char expectedPacket[] = {PACKET_START_BYTE, PACKET_UPDATE_FREQ, 0x00, 0xFF, 0xFF, PACKET_END_BYTE};
    uint8_t packetLength = 0;
    
    packetStatus_t result = packet_compile(testPacketBuf, testPayloadBuf, packetLength, PACKET_UPDATE_FREQ);

    TEST_ASSERT_EQUAL(PACKET_VALID, result);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expectedPacket, testPacketBuf, 6);
}

void test_packet_compile_builds_packet(void) {
    char testPacketBuf[9] = {0};
    char testPayloadBuf[] = {0x00, 0x01, 0x02};
    char expectedPacket[] = {PACKET_START_BYTE, PACKET_UPDATE_FREQ, 0x03, 0x00, 0x01, 0x02, 0xDF, 0xEF, PACKET_END_BYTE};
    uint8_t packetLength = 3;
    
    packetStatus_t result = packet_compile(testPacketBuf, testPayloadBuf, packetLength, PACKET_UPDATE_FREQ);

    TEST_ASSERT_EQUAL(PACKET_VALID, result);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expectedPacket, testPacketBuf, 9);
}

void test_packet_compile_builds_packet_null_payload_fails(void) {
    char testPacketBuf[6] = {0};
    char* testPayloadBuf = NULL;
    char expectedPacket[] = {PACKET_START_BYTE, PACKET_UPDATE_FREQ, 0x00, 0xFF, 0xFF, PACKET_END_BYTE};
    uint8_t packetLength = 5;
    
    packetStatus_t result = packet_compile(testPacketBuf, testPayloadBuf, packetLength, PACKET_UPDATE_FREQ);

    TEST_ASSERT_EQUAL(PACKET_UNKOWN_ERROR, result);
}