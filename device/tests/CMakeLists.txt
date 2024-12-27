set(SRC_DIR ../target/src)
set(MOCKS_DIR ./mocks)
# set(AVR_EXTENDS_DIR ../target/libs/avr-extends)

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(radioSoftwareTests LANGUAGES C)

enable_testing()
add_subdirectory(unity)

add_unity_test(test_packet_handler test_packet_handler.c ${SRC_DIR}/packet_handler.c)
target_include_directories(test_packet_handler PRIVATE ${UNITY_DIR} ${SRC_DIR})