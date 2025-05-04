cmake_minimum_required(VERSION 3.14)
# Set app name(s) and version(s)
# Set project name and version
project(bno055_calibration VERSION 0.1)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Directory names and path
add_executable(bno055_calibration
    bno055_calibration.cpp
)

# Link the test application with the PCA9685 library and other dependencies
target_link_libraries(bno055_calibration PUBLIC
    bno055
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
    flash_manager
)

# Include directories for the test application
target_include_directories(bno055_calibration PUBLIC
    ${BNO055}
)

# Additional compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra ")

# Enable/disable STDIO via USB and UART for the test application
pico_enable_stdio_usb(bno055_calibration 1)
pico_enable_stdio_uart(bno055_calibration 1)

# Enable extra build products for the test application
pico_add_extra_outputs(bno055_calibration)



# Calibration check

# Directory names and path
add_executable(bno055_calibration_check
    bno055_calibration_check.cpp
)

# Link the test application with the PCA9685 library and other dependencies
target_link_libraries(bno055_calibration_check PUBLIC
    bno055
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
    flash_manager
)

# Include directories for the test application
target_include_directories(bno055_calibration_check PUBLIC
    ${BNO055}
)

# Additional compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra ")

# Enable/disable STDIO via USB and UART for the test application
pico_enable_stdio_usb(bno055_calibration_check 1)
pico_enable_stdio_uart(bno055_calibration_check 1)

# Enable extra build products for the test application
pico_add_extra_outputs(bno055_calibration_check)
