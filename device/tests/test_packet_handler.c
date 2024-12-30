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
    
    packetStatus_t result = packet_validate(testPacket);

    TEST_ASSERT_EQUAL(PACKET_SCHEMA_ERROR, result);
}

void test_packet_validate_start_end_returns_fail(void) {
    char testPacket[3] = {0};

    testPacket[0] = PACKET_START_BYTE;
    testPacket[1] = PACKET_END_BYTE;

    packetStatus_t result = packet_validate(testPacket);

    TEST_ASSERT_EQUAL(PACKET_SCHEMA_ERROR, result);
}

void test_packet_validate_start_end_type_crc_returns_fail(void) {
    char testPacket[6] = {0};

    testPacket[0] = PACKET_START_BYTE;
    testPacket[PACKET_IDENTIFIER_LOC] = PACKET_HEARTBEAT;
    testPacket[2] = 0x10;
    testPacket[3] = 0x12;
    testPacket[4] = PACKET_END_BYTE;

    packetStatus_t result = packet_validate(testPacket);

    TEST_ASSERT_EQUAL(PACKET_LENGTH_ERROR, result);
}

void test_packet_validate_empty_packet_returns_fail(void) {
    char testPacket[7] = {0};

    testPacket[0] = PACKET_START_BYTE;
    testPacket[PACKET_IDENTIFIER_LOC] = PACKET_HEARTBEAT;
    testPacket[PACKET_LENGTH_LOC] = 0x00;
    testPacket[3] = 0x10;
    testPacket[4] = 0x12;    
    testPacket[5] = PACKET_END_BYTE;

    packetStatus_t result = packet_validate(testPacket);

    TEST_ASSERT_EQUAL(PACKET_SCHEMA_ERROR, result);
}

void test_packet_validate_full_packet_returns_valid(void) {
    char testPacket[8] = {0};

    testPacket[0] = PACKET_START_BYTE;
    testPacket[PACKET_IDENTIFIER_LOC] = PACKET_HEARTBEAT;
    testPacket[PACKET_LENGTH_LOC] = 0x01;
    testPacket[3] = 0x01;
    testPacket[4] = 0x10;
    testPacket[5] = 0x12;    
    testPacket[6] = PACKET_END_BYTE;

    packetStatus_t result = packet_validate(testPacket);

    TEST_ASSERT_EQUAL(PACKET_VALID, result);
}