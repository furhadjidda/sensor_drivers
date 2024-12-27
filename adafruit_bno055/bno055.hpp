

#ifndef BNO055_HPP_
#define BNO055_HPP_
#include <cstdio>

#include "bno055_common.hpp"
#include "hardware/i2c.h"

namespace bno055_sensor {
class Bno055 {
   public:
    bool initialization();
    uint8_t get_temp();
    void get_euler_angles(EulerData& euler_data);

   private:
    void bno055_write_register(uint8_t reg, uint8_t value);
    uint8_t bno055_read_register(uint8_t reg);
    void bno055_read_bytes(uint8_t reg, uint8_t* buffer, size_t length);
};
}  // namespace bno055_sensor
#endif BNO055_HPP_