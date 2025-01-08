
#include "bno055.hpp"

// I2C Configuration
#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5

namespace bno055_sensor {

bool Bno055::initialization() {
    // Initialize I2C bus
    i2c_init(I2C_PORT, 100 * 1000); // 100 kHz I2C speed
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("Initializing BNO055...\n");

    // Check if the BNO055 is connected
    uint8_t chip_id = bno055_read_register(BNO055_CHIP_ID_ADDR);
    if (chip_id != BNO055_ID) {
        printf("BNO055 not detected! Chip ID: 0x%02X\n", chip_id);
        return false; // Initialization failed
    }
    printf("BNO055 detected! Chip ID: 0x%02X\n", chip_id);

    // Perform a soft reset
    bno055_write_register(BNO055_SYS_TRIGGER_ADDR, 0x20);
    sleep_ms(650); // Wait for the reset to complete

    // Verify chip ID again after reset
    chip_id = bno055_read_register(BNO055_CHIP_ID_ADDR);
    if (chip_id != BNO055_ID) {
        printf("BNO055 not responding after reset! Chip ID: 0x%02X\n", chip_id);
        return false; // Initialization failed
    }

    // Set the operating mode to CONFIG_MODE for initial setup
    bno055_write_register(BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG);
    sleep_ms(30);

    // Perform any necessary sensor configuration (e.g., power mode, unit selection)
    // Example: Set the power mode to normal
    bno055_write_register(BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
    sleep_ms(30);

    // Set the sensor to NDOF mode
    bno055_write_register(BNO055_OPR_MODE_ADDR, OPERATION_MODE_NDOF);
    sleep_ms(30);

    printf("BNO055 initialization complete.\n");
    return true; // Initialization successful
}

uint8_t Bno055::get_temp() { return bno055_read_register(BNO055_TEMP_ADDR); }

void Bno055::get_euler_angles(EulerData &euler_data) { bno055_read_bytes(BNO055_EULER_H_LSB_ADDR, euler_data, 6); }

void Bno055::get_vector(vector_type_t vector_type, double data[3]) {

    uint8_t buffer[6] = {0};

    int16_t x, y, z;
    x = y = z = 0;

    /* Read vector data (6 bytes) */
    bno055_read_bytes((bno055_reg_t)vector_type, buffer, 6);

    x = ((int16_t)buffer[0]) | (((int16_t)buffer[1]) << 8);
    y = ((int16_t)buffer[2]) | (((int16_t)buffer[3]) << 8);
    z = ((int16_t)buffer[4]) | (((int16_t)buffer[5]) << 8);

    /*!
     * Convert the value to an appropriate range (section 3.6.4)
     * and assign the value to the Vector type
     */
    double scale = 1.0;
    switch (vector_type) {
    case VECTOR_MAGNETOMETER:
    case VECTOR_GYROSCOPE:
    case VECTOR_EULER:
        scale = 16.0;
        break;
    case VECTOR_ACCELEROMETER:
    case VECTOR_GRAVITY:
    case VECTOR_LINEARACCEL:
        scale = 100.0;
        break;
    }

    data[0] = ((double)x) / scale;
    data[1] = ((double)y) / scale;
    data[2] = ((double)z) / scale;
}

void Bno055::get_quaternion(quaternion_data &quaternion_data) {
    uint8_t buffer[8] = {0}; // Quaternion data is 8 bytes (4 components, 2 bytes each)

    // Read 8 bytes starting at the quaternion data register
    bno055_read_bytes(BNO055_QUATERNION_DATA_W_LSB_ADDR, buffer, 8);

    // Convert the raw data into signed integers
    int16_t w = ((int16_t)buffer[0]) | (((int16_t)buffer[1]) << 8);
    int16_t x = ((int16_t)buffer[2]) | (((int16_t)buffer[3]) << 8);
    int16_t y = ((int16_t)buffer[4]) | (((int16_t)buffer[5]) << 8);
    int16_t z = ((int16_t)buffer[6]) | (((int16_t)buffer[7]) << 8);

    // Scale the values to the range specified in the datasheet
    const double scale = 1.0 / 16384.0;

    quaternion_data.w = w * scale;
    quaternion_data.x = x * scale;
    quaternion_data.y = y * scale;
    quaternion_data.z = z * scale;
}

void Bno055::get_system_status(uint8_t *system_status, uint8_t *self_test_result, uint8_t *system_error) {
    // Configure BNO055
    bno055_write_register(BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG);
    sleep_ms(100);

    bno055_write_register(BNO055_PAGE_ID_ADDR, 0);

    /* System Status (see section 4.3.58)
       0 = Idle
       1 = System Error
       2 = Initializing Peripherals
       3 = System Iniitalization
       4 = Executing Self-Test
       5 = Sensor fusio algorithm running
       6 = System running without fusion algorithms
     */

    if (system_status != 0)
        *system_status = bno055_read_register(BNO055_SYS_STAT_ADDR);

    /* Self Test Results
       1 = test passed, 0 = test failed

       Bit 0 = Accelerometer self test
       Bit 1 = Magnetometer self test
       Bit 2 = Gyroscope self test
       Bit 3 = MCU self test

       0x0F = all good!
     */

    if (self_test_result != 0)
        *self_test_result = bno055_read_register(BNO055_SELFTEST_RESULT_ADDR);

    /* System Error (see section 4.3.59)
       0 = No error
       1 = Peripheral initialization error
       2 = System initialization error
       3 = Self test result failed
       4 = Register map value out of range
       5 = Register map address out of range
       6 = Register map write error
       7 = BNO low power mode not available for selected operat ion mode
       8 = Accelerometer power mode not available
       9 = Fusion algorithm configuration error
       A = Sensor configuration error
     */

    if (system_error != 0)
        *system_error = bno055_read_register(BNO055_SYS_ERR_ADDR);

    sleep_ms(200);

    bno055_write_register(BNO055_OPR_MODE_ADDR, OPERATION_MODE_NDOF);
    sleep_ms(100);
}

void Bno055::check_firmware_version() {
    uint8_t sw_major = bno055_read_register(BNO055_SW_REV_ID_LSB_ADDR);
    uint8_t sw_minor = bno055_read_register(BNO055_SW_REV_ID_MSB_ADDR);
    printf("Firmware Version: %u.%u\n", sw_major, sw_minor);
}

void Bno055::get_calibration(uint8_t *sys, uint8_t *gyro, uint8_t *accel, uint8_t *mag) {
    uint8_t calData = bno055_read_register(BNO055_CALIB_STAT_ADDR);
    if (sys != NULL) {
        *sys = (calData >> 6) & 0x03;
    }
    if (gyro != NULL) {
        *gyro = (calData >> 4) & 0x03;
    }
    if (accel != NULL) {
        *accel = (calData >> 2) & 0x03;
    }
    if (mag != NULL) {
        *mag = calData & 0x03;
    }
}

bool Bno055::is_fully_calibrated() {
    uint8_t system, gyro, accel, mag;
    get_calibration(&system, &gyro, &accel, &mag);
    printf("system: %x gyro %x accel %x mag %x\n", system, gyro, accel, mag);
    switch (mMode) {
    case OPERATION_MODE_ACCONLY:
        return (accel == 3);
    case OPERATION_MODE_MAGONLY:
        return (mag == 3);
    case OPERATION_MODE_GYRONLY:
    case OPERATION_MODE_M4G: /* No magnetometer calibration required. */
        return (gyro == 3);
    case OPERATION_MODE_ACCMAG:
    case OPERATION_MODE_COMPASS:
        return (accel == 3 && mag == 3);
    case OPERATION_MODE_ACCGYRO:
    case OPERATION_MODE_IMUPLUS:
        return (accel == 3 && gyro == 3);
    case OPERATION_MODE_MAGGYRO:
        return (mag == 3 && gyro == 3);
    default:
        return (system == 3 && gyro == 3 && accel == 3 && mag == 3);
    }
}

void Bno055::set_ext_crystal_use(bool usextal) {
    /* Switch to config mode (just in case since this is the default) */
    bno055_write_register(BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG);
    sleep_ms(25);
    bno055_write_register(BNO055_PAGE_ID_ADDR, 0);
    if (usextal) {
        bno055_write_register(BNO055_SYS_TRIGGER_ADDR, 0x80);
    } else {
        bno055_write_register(BNO055_SYS_TRIGGER_ADDR, 0x00);
    }
    sleep_ms(10);
    /* Set the requested operating mode (see section 3.3) */
    bno055_write_register(BNO055_OPR_MODE_ADDR, OPERATION_MODE_NDOF);
    sleep_ms(20);
}

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
void Bno055::bno055_read_bytes(uint8_t reg, uint8_t *buffer, size_t length) {
    i2c_write_blocking(I2C_PORT, BNO055_ADDRESS_A, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, BNO055_ADDRESS_A, buffer, length, false);
}

} // namespace bno055_sensor