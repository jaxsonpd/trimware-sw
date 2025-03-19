add_executable(radio-software main.c device_select.c freq_input.c freq_info.c freq_display.c TM1638.c TM1637.c freq_handler.c)

target_compile_options(radio-software PRIVATE -Os -DF_CPU=16000000UL -mmcu=atmega328p -Wall -Wstrict-prototypes -Wextra)
target_link_libraries(radio-software PRIVATE avr-extends)
target_link_libraries(radio-software PRIVATE custom-can-protocol)

add_compile_definitions(F_CPU=16000000UL)

add_program_target(radio-software PROGRAM_TARGET program)