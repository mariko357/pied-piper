cmake_minimum_required(VERSION 3.12)

set(NAME pied-piper)
include(cmake/pico_sdk_import.cmake)

project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${NAME}
        src/main.cpp
        src/player.cpp
        src/core1.cpp
        src/buttons.cpp
        src/display.cpp
)

include_directories(
        include)

# pull in common dependencies
target_link_libraries(${NAME} pico_stdlib hardware_i2c pico_multicore)
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

string(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")
#target_compile_definitions(${NAME} PRIVATE PICO_DEFAULT_UART_BAUD_RATE=921600)
