cmake_minimum_required(VERSION 3.14)
# Set app name(s) and version(s)
# Set project name and version
project(bno055_example VERSION 0.1)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Directory names and path
add_executable(bno055_example
    bno055_example.cpp
    ${BNO055}/bno055.cpp
)

# Link the test application with the PCA9685 library and other dependencies
target_link_libraries(bno055_example PUBLIC
    #bno055
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
)

# Include directories for the test application
target_include_directories(bno055_example PUBLIC
    ${BNO055}
    ${BNO055}/utility
)

# Additional compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -mcpu=cortex-m0 -mthumb")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -mcpu=cortex-m0 -mthumb")

# Enable/disable STDIO via USB and UART for the test application
pico_enable_stdio_usb(bno055_example 1)
pico_enable_stdio_uart(bno055_example 1)

# Enable extra build products for the test application
pico_add_extra_outputs(bno055_example)
