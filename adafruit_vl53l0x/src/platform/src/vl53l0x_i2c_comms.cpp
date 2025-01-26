#include <cstdio>

#include "../../vl53l0x_def.h"
#include "../../vl53l0x_i2c_platform.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
// #define I2C_DEBUG

int VL53L0X_i2c_init(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baudrate) {
    i2c_init(i2c, baudrate);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    printf("Initializing BNO055...\n");
    return VL53L0X_ERROR_NONE;
}

int VL53L0X_write_multi(uint8_t deviceAddress, uint8_t index, uint8_t *pdata, uint32_t count, i2c_inst_t *i2c) {
    uint8_t buffer[count + 1];
    buffer[0] = index;
    for (uint32_t i = 0; i < count; i++) {
        buffer[i + 1] = pdata[i];
    }

#ifdef I2C_DEBUG
    printf("\tWriting %lu bytes to addr 0x%02X: ", count, index);
    for (uint32_t i = 0; i < count; i++) {
        printf("0x%02X, ", pdata[i]);
    }
    printf("\n");
#endif

    int ret = i2c_write_blocking(i2c, deviceAddress, buffer, count + 1, false);
    return (ret < 0) ? VL53L0X_ERROR_CONTROL_INTERFACE : VL53L0X_ERROR_NONE;
}

int VL53L0X_read_multi(uint8_t deviceAddress, uint8_t index, uint8_t *pdata, uint32_t count, i2c_inst_t *i2c) {
    int ret = i2c_write_blocking(i2c, deviceAddress, &index, 1, true);
    if (ret < 0) {
        return VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    ret = i2c_read_blocking(i2c, deviceAddress, pdata, count, false);
    if (ret < 0) {
        return VL53L0X_ERROR_CONTROL_INTERFACE;
    }

#ifdef I2C_DEBUG
    printf("\tReading %lu bytes from addr 0x%02X: ", count, index);
    for (uint32_t i = 0; i < count; i++) {
        printf("0x%02X, ", pdata[i]);
    }
    printf("\n");
#endif

    return VL53L0X_ERROR_NONE;
}

int VL53L0X_write_byte(uint8_t deviceAddress, uint8_t index, uint8_t data, i2c_inst_t *i2c) {
    return VL53L0X_write_multi(deviceAddress, index, &data, 1, i2c);
}

int VL53L0X_write_word(uint8_t deviceAddress, uint8_t index, uint16_t data, i2c_inst_t *i2c) {
    uint8_t buff[2];
    buff[1] = data & 0xFF;
    buff[0] = data >> 8;
    return VL53L0X_write_multi(deviceAddress, index, buff, 2, i2c);
}

int VL53L0X_write_dword(uint8_t deviceAddress, uint8_t index, uint32_t data, i2c_inst_t *i2c) {
    uint8_t buff[4];
    buff[3] = data & 0xFF;
    buff[2] = (data >> 8) & 0xFF;
    buff[1] = (data >> 16) & 0xFF;
    buff[0] = (data >> 24) & 0xFF;
    return VL53L0X_write_multi(deviceAddress, index, buff, 4, i2c);
}

int VL53L0X_read_byte(uint8_t deviceAddress, uint8_t index, uint8_t *data, i2c_inst_t *i2c) {
    return VL53L0X_read_multi(deviceAddress, index, data, 1, i2c);
}

int VL53L0X_read_word(uint8_t deviceAddress, uint8_t index, uint16_t *data, i2c_inst_t *i2c) {
    uint8_t buff[2];
    int ret = VL53L0X_read_multi(deviceAddress, index, buff, 2, i2c);

    if (ret == VL53L0X_ERROR_NONE) {
        *data = (buff[0] << 8) | buff[1];
    }

    return ret;
}

int VL53L0X_read_dword(uint8_t deviceAddress, uint8_t index, uint32_t *data, i2c_inst_t *i2c) {
    uint8_t buff[4];
    int ret = VL53L0X_read_multi(deviceAddress, index, buff, 4, i2c);

    if (ret == VL53L0X_ERROR_NONE) {
        *data = (buff[0] << 24) | (buff[1] << 16) | (buff[2] << 8) | buff[3];
    }

    return ret;
}
