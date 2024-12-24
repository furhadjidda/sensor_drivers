cmake_minimum_required(VERSION 3.14)

# Set project name and version
project(Adafruit_Gps_Library VERSION 0.0)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Directory names and path
set(GPS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")


if (BUILD_FOR_HOST)
# Include app source code file(s)
add_library(Adafruit_Gps_Library
    src/Adafruit_GPS.cpp
    src/NMEA_build.cpp
    src/NMEA_data.cpp
    src/NMEA_parse.cpp
    ${MOCKS_PATH}/mock_i2c.cpp
)

# Define compile-time constants
target_compile_definitions(Adafruit_Gps_Library PUBLIC
    APP_NAME="${PROJECT_NAME}"
    APP_VERSION="${PROJECT_VERSION}"
    BUILD_NUM=${BUILD_NUMBER}
)
# Link to built libraries
else()
# Include app source code file(s)
add_library(Adafruit_Gps_Library
    src/Adafruit_GPS.cpp
    src/NMEA_build.cpp
    src/NMEA_data.cpp
    src/NMEA_parse.cpp
)

# Define compile-time constants
target_compile_definitions(Adafruit_Gps_Library PUBLIC
    APP_NAME="${PROJECT_NAME}"
    APP_VERSION="${PROJECT_VERSION}"
    BUILD_NUM=${BUILD_NUMBER}
)

# Link to built libraries
target_link_libraries(Adafruit_Gps_Library PUBLIC
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
)

endif()
# Include directories
target_include_directories(Adafruit_Gps_Library PUBLIC
    ${GPS_SRC_DIR}
    ${GPS_SRC_DIR}/src
    ${MOCKS_PATH}
)

# Set compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")


if ( NOT BUILD_FOR_HOST)
# Add example subdirectory
add_subdirectory(${GPS_SRC_DIR}/examples/GPS_I2C_Parsing)
endif()
