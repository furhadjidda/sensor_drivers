cmake_minimum_required(VERSION 3.14)

# Set project name and version
project(GPS_I2C_Parsing VERSION 0.0)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Directory names and path
set(GPS_I2C_PARSING_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Include app source code file(s)
add_executable(${PROJECT_NAME}
    GPS_I2C_Parsing.cpp
)

# Define compile-time constants
target_compile_definitions(${PROJECT_NAME} PUBLIC
    APP_NAME="${PROJECT_NAME}"
    APP_VERSION="${PROJECT_VERSION}"
    BUILD_NUM=${BUILD_NUMBER}
)

# Link to built libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
    Adafruit_Gps_Library
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${GPS_I2C_PARSING_DIR}
)

# Set compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")

# Enable/disable STDIO via USB and UART
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# Enable extra build products
pico_add_extra_outputs(${PROJECT_NAME})
