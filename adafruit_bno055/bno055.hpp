

#ifndef BNO055_HPP_
#define BNO055_HPP_
#include <cstdio>

#include "bno055_common.hpp"
#include "hardware/i2c.h"
#include <vector>

namespace bno055_sensor {
class Bno055 {
  public:
    bool initialization();
    uint8_t get_temp();
    void get_euler_angles(EulerData &euler_data);
    void get_system_status(uint8_t *system_status, uint8_t *self_test_result, uint8_t *system_error);
    void get_vector(vector_type_t vector_type, double data[3]);
    bool is_fully_calibrated();
    void get_calibration(uint8_t *sys, uint8_t *gyro, uint8_t *accel, uint8_t *mag);
    void check_firmware_version();
    void set_ext_crystal_use(bool usextal);
    void get_quaternion(quaternion_data &quaternion_data);

  private:
    void bno055_write_register(uint8_t reg, uint8_t value);
    uint8_t bno055_read_register(uint8_t reg);
    void bno055_read_bytes(uint8_t reg, uint8_t *buffer, size_t length);
    bno055_opmode_t mMode;
};
} // namespace bno055_sensor
#endif