cmake_minimum_required(VERSION 3.14)

# Set project name and version
project(vl53l0x VERSION 0.1)

# Set C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Source directories
set(VL53L0X_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(VL53L0X_CORE_SRC_DIR "${VL53L0X_SRC_DIR}/core/src")
set(VL53L0X_PLATFORM_SRC_DIR "${VL53L0X_SRC_DIR}/platform/src")

###################################################################
# Building a library
###################################################################

# Collect source files
file(GLOB VL53L0X_CORE_SOURCES "${VL53L0X_CORE_SRC_DIR}/*.cpp")
file(GLOB VL53L0X_PLATFORM_SOURCES "${VL53L0X_PLATFORM_SRC_DIR}/*.cpp")

# Add the library
add_library(vl53l0x STATIC
    ${VL53L0X_CORE_SOURCES}
    ${VL53L0X_PLATFORM_SOURCES}
    src/Adafruit_VL53L0X.cpp
)

# Include directories for the library
target_include_directories(vl53l0x PUBLIC
    "${VL53L0X_SRC_DIR}"  # Add top-level source directory
)

# Link the library with dependencies
target_link_libraries(vl53l0x PUBLIC
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
)

###################################################################
# Optional: Add example directory (if needed)
###################################################################
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/examples")
