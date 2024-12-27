#include <cstdio>

#include "bno055.hpp"
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
    bno055_sensor::Bno055* bno055 = new bno055_sensor::Bno055();
    bno055->initialization();
    /* Initialise the sensor */

    while (true) {
        int8_t temp = bno055->get_temp();
        printf("Temperature: %d°C\n", temp);

        EulerData euler_data;
        bno055->get_euler_angles(euler_data);
        int16_t heading = (euler_data[1] << 8) | euler_data[0];
        int16_t roll = (euler_data[3] << 8) | euler_data[2];
        int16_t pitch = (euler_data[5] << 8) | euler_data[4];
        printf("Heading: %.2f°\n", heading / 16.0);
        printf("Roll: %.2f°\n", roll / 16.0);
        printf("Pitch: %.2f°\n", pitch / 16.0);
        sleep_ms(500);  // Wait 500 ms
    }
    return 0;
}