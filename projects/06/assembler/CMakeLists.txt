cmake_minimum_required(VERSION 3.10.2)

project (assembler)

file (GLOB SOURCES "*.cpp" "include/*")

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable (system ${SOURCES})
