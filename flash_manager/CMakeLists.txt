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