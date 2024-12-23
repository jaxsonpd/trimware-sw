set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

if(LINUX)
    set(CMAKE_SYSROOT /usr/lib/avr-gcc CACHE PATH "Path to sysroot")
endif()

find_program(CMAKE_C_COMPILER avr-gcc REQUIRED)
find_program(CMAKE_ASM_COMPILER avr-gcc REQUIRED)
find_program(CMAKE_CXX_COMPILER avr-g++ REQUIRED)
find_program(CMAKE_AS avr-as REQUIRED)
find_program(CMAKE_AR avr-ar REQUIRED)
find_program(CMAKE_OBJCOPY avr-objcopy REQUIRED)
find_program(CMAKE_OBJDUMP avr-objdump REQUIRED)
find_program(CMAKE_SIZE avr-size REQUIRED)

set(
    CMAKE_C_FLAGS
        "-DF_CPU=16000000UL \
        -mmcu=atmega328p"
    CACHE INTERNAL "C compiler flags"
)

