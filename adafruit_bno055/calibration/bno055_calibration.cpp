#include <cstdio>
#include <vector>

#include "bno055.hpp"
#include "flash_manager.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
int main() {
    stdio_init_all();
    printf("starting driver\n");
    // Note to self , create this object on heap. creating on stack causes the
    // program to crash
    bno055_sensor::Bno055 *bno055 = new bno055_sensor::Bno055();
    bno055->initialization();
    FlashManager flash_manager;
    uint8_t calibration_counter = 0;

    bool once = false;
    while (true) {
        int8_t temp = bno055->get_temp();
        printf("Temperature: %d°C\n", temp);

        // System Status
        if (!once) {
            uint8_t system = 0;
            uint8_t seltest = 0;
            uint8_t error = 0;
            bno055->get_system_status(&system, &seltest, &error);
            printf("system: %x selft test %x error %x \n", system, seltest, error);
            bno055->check_firmware_version();
            sleep_ms(500);  // Wait 500 ms
            once = true;
        }

        // Getting IMU data
        double data[3] = {};
        bno055->get_vector(VECTOR_EULER, data);
        if (true == bno055->is_fully_calibrated()) {
            printf("system fully calibrated \n");
            ++calibration_counter;
        }

        if (calibration_counter == 10) {
            CalibrationData calibration_data = {};
            printf("system fully calibrated ..Saving the data to flash\n");
            flash_manager.write_data(calibration_data, sizeof(calibration_data));
            exit(0);
        }
        sleep_ms(20);
    }
    return 0;
}