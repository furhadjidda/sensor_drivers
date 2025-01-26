
#include <cstdio>
#include <vector>

#include "Adafruit_VL53L0X.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    printf("starting driver\n");
    // Note to self , create this object on heap. creating on stack causes the
    // program to crash
    VL53L0X_RangingMeasurementData_t measure;
    Adafruit_VL53L0X lox = Adafruit_VL53L0X();

    if (!lox.begin()) {
        printf("Failed to boot VL53L0X");
        return 1;
    }

    while (1) {
        lox.startRangeContinuous();
        if (lox.isRangeComplete()) {
            printf("Distance in mm: %d\n", lox.readRange());
        }
        lox.rangingTest(&measure, false);  // pass in 'true' to get debug data printout!
        if (measure.RangeStatus != 4) {    // phase failures have incorrect data
            printf(">> Distance (mm): %d\n", measure.RangeMilliMeter);
        } else {
            printf(" out of range ");
        }
        sleep_ms(1000);
    }
}