
/*
 *   This file is part of sensor driver project.
 *
 *   embedded software pico playground projec is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 *   embedded software pico playground project is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License v3.0
 *   along with embedded software pico playground project.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef SSD1306OLED_h
#define SSD1306OLED_h

// SSD1306 OLED I2C driver for Raspberry Pi Pico board, derived from DisplayDriver
// generic base graphics class
// defines a memory buffer for a 128 pixel wide by 64 pixel high Display
// pixels arranged in the buffer as per the SSD1306 memory pattern, each byte
// is 8 vertical pixels, next byte is the next column of 8 horizontally

#include <stdlib.h>

#include "display_driver.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

// control constants for the SSD1306
#define SET_CONTRAST 0x81
#define SET_ENTIRE_ON 0xA4
#define SET_NORM_INV 0xA6
#define SET_DISP 0xAE
#define SET_MEM_ADDR 0x20
#define SET_COL_ADDR 0x21
#define SET_PAGE_ADDR 0x22
#define SET_DISP_START_LINE 0x40
#define SET_SEG_REMAP 0xA0
#define SET_MUX_RATIO 0xA8
#define SET_COM_OUT_DIR 0xC0
#define SET_DISP_OFFSET 0xD3
#define SET_COM_PIN_CFG 0xDA
#define SET_DISP_CLK_DIV 0xD5
#define SET_PRECHARGE 0xD9
#define SET_VCOM_DESEL 0xDB
#define SET_CHARGE_PUMP 0x8D

class SSD1306Oled : public DisplayDriver {
   public:
    // class constructor, i2C address typically 0x3C
    SSD1306Oled(i2c_inst_t *i2c, uint SDA, uint SCL, uint8_t i2c_address);

    // call .Begin() prior to any other calls, this sets up I2C and the associated pin functions,
    // allocates the screen buffer etc
    void Begin(void);

    // this is the _only_ function that actually writes to the screen, call after drawing operations
    void Display(void);

    // set the entire screen buffer to black (unilluminated pixels)
    void ClearAll(void);

    // return the value of a given pixel
    uint8_t GetPixel(int16_t X, int16_t Y);

    // set the value of a given pixel, returns its previous value
    uint8_t SetPixel(int16_t X, int16_t Y, uint8_t V);

   private:
    i2c_inst_t *i2c_;
    uint sda_;
    uint scl_;
    uint8_t i2c_address_;
    uint8_t height_ = 64;
    uint8_t width_ = 128;

    uint16_t display_buffer_size_;  // size of Display buffer, in bytes
    uint8_t *display_buffer_;

    void WriteCommand(uint8_t cmd);
};

#endif
