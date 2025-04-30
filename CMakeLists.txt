cmake_minimum_required(VERSION 3.14)

add_subdirectory("${CMAKE_SOURCE_DIR}/sensor_drivers/pca9685")
add_subdirectory("${CMAKE_SOURCE_DIR}/sensor_drivers/adafruit_pa1010d_mini_gps")
add_subdirectory("${CMAKE_SOURCE_DIR}/sensor_drivers/adafruit_bno055")
add_subdirectory("${CMAKE_SOURCE_DIR}/sensor_drivers/adafruit_vl53l0x")
add_subdirectory("${CMAKE_SOURCE_DIR}/sensor_drivers/oled_display")
add_subdirectory("${CMAKE_SOURCE_DIR}/sensor_drivers/flash_manager")