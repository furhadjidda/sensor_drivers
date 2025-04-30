#ifndef flash_manager_hpp
#define flash_manager_hpp
#include <stdio.h>

#include <cstring>
#include <iostream>

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

class FlashManager {
   public:
    void write_data(const uint8_t *data, size_t size);
    void read_data(uint8_t *data, size_t size);

   private:
    uint32_t compute_crc32(const void *data, size_t length);
};
#endif