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

void setUp(void) {
   
}

void tearDown(void) {
   
}

// =========================== Tests ===========================
void test_packet_validate_returns_empty_fail(void) {
    char* testPacket = "";
    packetStatus_t result = packet_validate(testPacket);

    TEST_ASSERT_EQUAL(PACKET_FRAMING_ERROR, result);
}