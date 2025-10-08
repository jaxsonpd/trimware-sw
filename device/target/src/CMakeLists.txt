add_executable(radio-software radio.c radio/device_select.c radio/freq_input.c radio/freq_info.c radio/freq_display.c radio/display_handler.c common/TM1638.c common/TM1637.c radio/freq_handler.c)

target_compile_options(radio-software PRIVATE -Os -DF_CPU=16000000UL -mmcu=atmega328p -Wall -Wstrict-prototypes -Wextra)
target_link_libraries(radio-software PRIVATE avr-extends)
target_link_libraries(radio-software PRIVATE custom-can-protocol)

add_compile_definitions(F_CPU=16000000UL)

add_program_target(radio-software PROGRAM_TARGET program)          

add_executable(ap-software autopilot.c common/TM1638.c)

target_compile_options(ap-software PRIVATE -Os -DF_CPU=16000000UL -mmcu=atmega328p -Wall -Wstrict-prototypes -Wextra)
target_link_libraries(ap-software PRIVATE avr-extends)
target_link_libraries(ap-software  PRIVATE custom-can-protocol)

add_compile_definitions(F_CPU=16000000UL)

add_program_target(ap-software  PROGRAM_TARGET program)      