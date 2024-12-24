// i2c_wrapper.h
#ifndef I2C_WRAPPER_H
#define I2C_WRAPPER_H

#ifdef BUILD_FOR_HOST
#include "mock_hardware.hpp"
#include "mock_i2c.hpp"
// typedef mock_i2c_inst_t i2c_inst_t;
#define i2c_read_blocking mock_i2c_read_blocking
#define i2c_write_blocking mock_i2c_write_blocking
#else
#include "hardware/i2c.h"  // Include the real Pico SDK I2C header
#endif

#endif  // I2C_WRAPPER_H
