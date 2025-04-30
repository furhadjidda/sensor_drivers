#include "flash_manager.hpp"

#include "hardware/flash.h"
// #define FLASH_TOTAL_SIZE (16 * 1024 * 1024)  // 16MB flash
// #define SAFE_FLASH_OFFSET (4 * 1024 * 1024)  // 4MB offset to stay safe
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define JOIN_MESSAGE(name, value) name " = " STR(value) " bytes"

#ifndef FLASH_TOTAL_SIZE
#error "FLASH_TOTAL_SIZE is not defined! Define via -DFLASH_TOTAL_SIZE=<bytes>"
#endif

#ifndef SAFE_FLASH_OFFSET
#error "SAFE_FLASH_OFFSET is not defined! Define via -DSAFE_FLASH_OFFSET=<bytes>"
#endif

// Optional: Emit compile-time message
#if FLASH_TOTAL_SIZE < (4 * 1024 * 1024)
#error "FLASH_TOTAL_SIZE is less than 4MB. Must be at least 4MB!"
#else
#pragma message(JOIN_MESSAGE("FLASH_TOTAL_SIZE", FLASH_TOTAL_SIZE))
#pragma message(JOIN_MESSAGE("SAFE_FLASH_OFFSET", SAFE_FLASH_OFFSET))
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

void FlashManager::write_data(const uint8_t *data, size_t size) {
    // Calculate CRC32 of the data
    uint32_t crc_written = compute_crc32(data, size);
    printf("CRC32 to write: 0x%08x\n", crc_written);

    // Prepare flash page (data + crc)
    uint8_t buffer[FLASH_PAGE_SIZE];
    memset(buffer, 0xFF, sizeof(buffer));  // Flash erased state is all 1's
    memcpy(buffer, data, size);
    memcpy(buffer + size, &crc_written, sizeof(crc_written));  // Store CRC after data

    // Erase sector
    uint32_t interrupts = save_and_disable_interrupts();
    flash_range_erase(SAFE_FLASH_OFFSET, FLASH_SECTOR_SIZE);
    restore_interrupts(interrupts);

    // Write page
    interrupts = save_and_disable_interrupts();
    flash_range_program(SAFE_FLASH_OFFSET, buffer, FLASH_PAGE_SIZE);
    restore_interrupts(interrupts);

    printf("Data written to flash at offset 0x%08x.\n", SAFE_FLASH_OFFSET);
}

void FlashManager::read_data(uint8_t *data, size_t size) {
    // Read back the data from flash
    const uint8_t *flash_memory = (const uint8_t *)(XIP_BASE + SAFE_FLASH_OFFSET);
    const uint32_t *read_data = reinterpret_cast<const uint32_t *>(flash_memory);
    uint32_t read_crc;
    memcpy(&read_crc, flash_memory + size, sizeof(read_crc));

    printf("Data read from flash:\n");
    for (size_t i = 0; i < size / sizeof(uint32_t); ++i) {
        printf("%u ", read_data[i]);
    }
    printf("\n");

    // Verify CRC
    uint32_t crc_read = compute_crc32(read_data, size);
    printf("CRC32 read: 0x%08x\n", crc_read);

    if (crc_read == read_crc) {
        printf("✅ CRC check PASSED. Data integrity OK.\n");
    } else {
        printf("❌ CRC check FAILED. Data corrupted!\n");
    }
}

// Software CRC32 function
uint32_t FlashManager::compute_crc32(const void *data, size_t length) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; ++i) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }

    return ~crc;
}