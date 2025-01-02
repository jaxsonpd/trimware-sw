add_executable(radio-software main.c freq_info.c packet_handler.c process_handler.c freq_process.c)

target_compile_options(radio-software PRIVATE -Os -DF_CPU=16000000UL -mmcu=atmega328p -Wall -Wstrict-prototypes -Wextra)
target_link_libraries(radio-software PRIVATE avr-extends)
add_compile_definitions(F_CPU=16000000UL)

add_program_target(radio-software PROGRAM_TARGET program)