cmake_minimum_required(VERSION 3.14)
# Set app name(s) and version(s)
# Set project name and version
project(pca9685_test VERSION 0.1)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Directory names and path
set(PCA9685 "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(pca9685_test
    pca9685_test.cpp
)

# Link the test application with the PCA9685 library and other dependencies
target_link_libraries(pca9685_test PUBLIC
    pca9685
)

# Include directories for the test application
target_include_directories(pca9685_test PUBLIC
    ${PCA9685}
)

# Additional compiler flags
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")

# Enable/disable STDIO via USB and UART for the test application
pico_enable_stdio_usb(pca9685_test 1)
pico_enable_stdio_uart(pca9685_test 1)

# Enable extra build products for the test application
pico_add_extra_outputs(pca9685_test)
