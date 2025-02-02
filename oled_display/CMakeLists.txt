cmake_minimum_required(VERSION 3.14)
# Set app name(s) and version(s)
# Set project name and version
project(oled_driver_1306 VERSION 0.1)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set( PROJECT_NAME "ssd 1306 oled firmware")

# Directory names and path
set(SSD1306OLED "${CMAKE_CURRENT_SOURCE_DIR}")

###################################################################
# Building a library
###################################################################

add_library(oled_driver_1306 STATIC
    display_driver.cpp
    SSD1306Oled.cpp
)

# Include directories for the library
target_include_directories(oled_driver_1306 PUBLIC
    ${SSD1306OLED}
)

# Link the test application with the BNO055 library and other dependencies
target_link_libraries(oled_driver_1306 PUBLIC
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
)

add_subdirectory("${SSD1306OLED}/example")
