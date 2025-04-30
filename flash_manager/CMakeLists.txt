cmake_minimum_required(VERSION 3.14)
# Set app name(s) and version(s)
# Set project name and version
project(flash_manager VERSION 0.1)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set( PROJECT_NAME "flash_manager firmware")

# Directory names and path
set(FLASH_MANAGER "${CMAKE_CURRENT_SOURCE_DIR}")

###################################################################
# Building a library
###################################################################

# Set default values if not provided
set(FLASH_TOTAL_SIZE 16777216 CACHE STRING "Total flash size in bytes (default 16MB)")
set(SAFE_FLASH_OFFSET 4194304 CACHE STRING "Safe flash offset in bytes (default 4MB)")

# Add the definitions to the compiler
add_compile_definitions(
    FLASH_TOTAL_SIZE=${FLASH_TOTAL_SIZE}
    SAFE_FLASH_OFFSET=${SAFE_FLASH_OFFSET}
)

# Check flash size and issue error if it's less than 4MB
if(${FLASH_TOTAL_SIZE} LESS 4194304)
    message(FATAL_ERROR "FLASH_TOTAL_SIZE is less than 4MB! Must be >= 4194304 bytes.")
else()
    message(STATUS "Using FLASH_TOTAL_SIZE = ${FLASH_TOTAL_SIZE} bytes")
    message(STATUS "Using SAFE_FLASH_OFFSET = ${SAFE_FLASH_OFFSET} bytes")
endif()

add_library(flash_manager STATIC
    flash_manager.cpp
)

# Include directories for the library
target_include_directories(flash_manager PUBLIC
    ${FLASH_MANAGER}
)

# Link the test application with the FLASH_MANAGER library and other dependencies
target_link_libraries(flash_manager PUBLIC
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
    hardware_flash
)