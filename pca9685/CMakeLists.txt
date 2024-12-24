cmake_minimum_required(VERSION 3.14)
# Set app name(s) and version(s)
# Set project name and version
project(pca9685 VERSION 0.1)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Directory names and path
set(PCA9685 "${CMAKE_CURRENT_SOURCE_DIR}")

# Define compile-time constants
add_compile_definitions(APP_NAME="${PROJECT_NAME}")

###################################################################
# Building a library
###################################################################

add_library(pca9685 STATIC
    pca9685.cpp
)

# Include directories for the library
target_include_directories(pca9685 PUBLIC
    ${PCA9685}
)

# Link the test application with the PCA9685 library and other dependencies
target_link_libraries(pca9685 PUBLIC
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
)

add_subdirectory(tests)
