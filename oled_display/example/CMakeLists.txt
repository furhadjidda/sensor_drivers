cmake_minimum_required(VERSION 3.14)
# Set app name(s) and version(s)
# Set project name and version
project(oled_1306_example VERSION 0.1)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Directory names and path
set(OLED1306 "${CMAKE_CURRENT_SOURCE_DIR}")

# Define compile-time constants
add_compile_definitions(APP_NAME="${PROJECT_NAME}")

###################################################################
# Building a Binary
###################################################################

add_executable(oled_1306_example
    example.cpp
)

# Include directories for the library
target_include_directories(oled_1306_example PUBLIC
    ${OLED1306}
)

# Link the test application with the PCA9685 library and other dependencies
target_link_libraries(oled_1306_example PUBLIC
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
    oled_driver_1306
)

# Additional compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra ")

# Enable/disable STDIO via USB and UART for the test application
pico_enable_stdio_usb(oled_1306_example 1)
pico_enable_stdio_uart(oled_1306_example 1)

# Enable extra build products for the test application
pico_add_extra_outputs(oled_1306_example)
