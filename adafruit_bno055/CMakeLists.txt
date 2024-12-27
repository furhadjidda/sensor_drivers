cmake_minimum_required(VERSION 3.14)
# Set app name(s) and version(s)
# Set project name and version
project(pca9685 VERSION 0.1)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set( PROJECT_NAME "bno055 firmware")

# Directory names and path
set(BNO055 "${CMAKE_CURRENT_SOURCE_DIR}")

###################################################################
# Building a library
###################################################################

add_library(bno055 STATIC
    bno055.cpp
)

# Include directories for the library
target_include_directories(bno055 PUBLIC
    ${BNO055}
)

# Link the test application with the BNO055 library and other dependencies
target_link_libraries(bno055 PUBLIC
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
)

add_subdirectory("${BNO055}/example")
