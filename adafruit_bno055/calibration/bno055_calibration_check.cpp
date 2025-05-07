#include <cstdio>
#include <vector>

#include "bno055.hpp"
#include "flash_manager.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    printf("starting driver\n");

    // Wait 1 second for the system to stabilize
    sleep_ms(1000);

    // Create BNO055 object on heap
    bno055_sensor::Bno055 *bno055 = new bno055_sensor::Bno055();
    bno055->initialization();
    bno055->set_ext_crystal_use(true);
    // Load calibration data from flash
    FlashManager flash_manager;
    CalibrationData calibration_data = {};
    flash_manager.read_data(calibration_data, sizeof(calibration_data));
    // Wait for sensor to stabilize
    sleep_ms(2000);
    bno055->set_calibration_data(calibration_data);
    sleep_ms(2000);

    // Print firmware version once
    uint8_t system = 0;
    uint8_t seltest = 0;
    uint8_t error = 0;
    bno055->get_system_status(&system, &seltest, &error);
    printf("system: %x self test %x error %x \n", system, seltest, error);
    bno055->check_firmware_version();

    // Main loop
    absolute_time_t last_status_time = get_absolute_time();
    while (true) {
        // Check if 2 seconds have passed
        if (absolute_time_diff_us(last_status_time, get_absolute_time()) >= 2 * 1000 * 1000) {
            printf("system: %x self test %x error %x \n", system, seltest, error);
            last_status_time = get_absolute_time();
            int8_t temp = bno055->get_temp();
            printf("Temperature: %d°C\n", temp);
            if (bno055->is_fully_calibrated()) {
                printf("system fully calibrated\n");
                bno055->get_calibration_data(calibration_data);
            }
        }
        bno055->get_system_status(&system, &seltest, &error);
        printf("system: %x self test %x error %x \n", system, seltest, error);
        // Getting IMU data
        double data[3] = {};
        bno055->get_vector(VECTOR_EULER, data);
        if (true == bno055->is_fully_calibrated()) {
            printf("system fully calibrated \n");
        }
        sleep_ms(500);
    }
    return 0;
}