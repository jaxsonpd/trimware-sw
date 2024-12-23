cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(FlightSimSoftware LANGUAGES C)

include("cmake/program.cmake")

add_subdirectory(libs/avr-extends)
add_subdirectory(src)