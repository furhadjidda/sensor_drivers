
#include "bno055.hpp"

// I2C Configuration
#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5

namespace bno055_sensor {

bool Bno055::initialization() {
    i2c_init(I2C_PORT, 100 * 1000);  // 100 kHz I2C speed
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    printf("Initializing BNO055...\n");

    // Check if the BNO055 is connected
    uint8_t chip_id = bno055_read_register(BNO055_CHIP_ID_ADDR);
    if (chip_id != BNO055_ID) {
        printf("BNO055 not detected! Chip ID: 0x%02X\n", chip_id);
        while (true) {
            sleep_ms(1000);
        }
    }
    printf("BNO055 detected! Chip ID: 0x%02X\n", chip_id);

    // Configure BNO055 (e.g., set to normal operation mode)
    bno055_write_register(0x3D, 0x0C);  // Set to NDOF mode
    sleep_ms(100);                      // Allow sensor to initialize
}
uint8_t Bno055::get_temp() { return bno055_read_register(BNO055_TEMP_ADDR); }

void Bno055::get_euler_angles(EulerData& euler_data) { bno055_read_bytes(BNO055_EULER_H_LSB_ADDR, euler_data, 6); }

void Bno055::bno055_write_register(uint8_t reg, uint8_t value) {
    uint8_t data[] = {reg, value};
    i2c_write_blocking(I2C_PORT, BNO055_ADDRESS_A, data, 2, false);
}
uint8_t Bno055::bno055_read_register(uint8_t reg) {
    i2c_write_blocking(I2C_PORT, BNO055_ADDRESS_A, &reg, 1, true);
    uint8_t value;
    i2c_read_blocking(I2C_PORT, BNO055_ADDRESS_A, &value, 1, false);
    return value;
}
void Bno055::bno055_read_bytes(uint8_t reg, uint8_t* buffer, size_t length) {
    i2c_write_blocking(I2C_PORT, BNO055_ADDRESS_A, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, BNO055_ADDRESS_A, buffer, length, false);
}

}  // namespace bno055_sensor