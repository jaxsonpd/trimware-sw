add_library(avr-extends GPIO/GPIO.c UART/UART.c wire/wire.c)

target_compile_options(avr-extends PRIVATE -Os -DF_CPU=16000000UL -mmcu=atmega328p)
target_include_directories(avr-extends PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})