find_program(AVRDUDE avrdude REQUIRED)
message(STATUS "Found avrdude: ${AVRDUDE}")

set(USBDEVICE /dev/ttyUSB0)

function(add_program_target EXECUTABLE_TARGET)
    cmake_parse_arguments(
        PARSE_ARGV 1 ARGS "" "PROGRAM_TARGET;DEBUG_TARGET" ""
    )
    if(NOT TARGET ${EXECUTABLE_TARGET})
        message(SEND_ERROR "No target name '${EXECUTABLE_TARGET}'")
    endif()

    set(EXE_PATH $<TARGET_FILE:${EXECUTABLE_TARGET}>)
    set(program_target program-${EXECUTABLE_TARGET})

    add_custom_target(
        ${program_target}
        DEPENDS ${EXE_PATH}
        COMMAND ${AVRDUDE} -F -V -c arduino -p ATMEGA328P -P ${USBDEVICE} -b 115200 -U flash:w:${EXE_PATH}
    )
endfunction()