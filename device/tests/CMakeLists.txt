set(SRC_DIR ../target/src)
set(MOCKS_DIR ./mocks)
set(AVR_EXTENDS_DIR ../target/libs/avr-extends)

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(EmbeddedTerminalTests LANGUAGES C)

enable_testing()
add_subdirectory(unity)

add_unity_test(test_getopt test_getopt.c ${SRC_DIR}/utils.c)
target_include_directories(test_getopt PRIVATE ${UNITY_DIR} ${SRC_DIR})

add_unity_test(test_command test_command.c ${SRC_DIR}/command.c)
target_include_directories(test_command PRIVATE ${UNITY_DIR} ${SRC_DIR} ${MOCKS_DIR})
