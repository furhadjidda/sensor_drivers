
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

#include "SSD1306Oled.hpp"

/**
 * @brief Constructor for the SSD1306Oled class.
 *
 * This constructor initializes an instance of the SSD1306Oled class with the specified I2C interface,
 * SDA and SCL pins, and I2C address.
 *
 * @param i2c Pointer to the I2C interface instance.
 * @param SDA The GPIO pin number for the SDA (data) line.
 * @param SCL The GPIO pin number for the SCL (clock) line.
 * @param i2c_address The I2C address of the OLED display.
 */
SSD1306Oled::SSD1306Oled(i2c_inst_t *i2c, uint SDA, uint SCL, uint8_t i2c_address) {
    i2c_ = i2c;
    sda_ = SDA;
    scl_ = SCL;
    i2c_address_ = i2c_address;

    return;
}

/**
 * @brief Initializes the SSD1306 OLED display.
 *
 * This function sets up the I2C communication and initializes the SSD1306 OLED display
 * with a predefined sequence of commands. It also allocates and initializes the display
 * buffer, and writes the initial display content.
 *
 * @note This function assumes that the I2C pins (sda_ and scl_) and the display height (height_)
 * are already configured.
 *
 * The initialization sequence includes:
 * - Setting the display off
 * - Setting memory addressing mode
 * - Setting display start line
 * - Setting segment re-map
 * - Setting multiplex ratio
 * - Setting COM output scan direction
 * - Setting display offset
 * - Setting display clock divide ratio/oscillator frequency
 * - Setting pre-charge period
 * - Setting VCOMH deselect level
 * - Setting contrast control
 * - Setting entire display on
 * - Setting normal display
 * - Enabling charge pump regulator
 * - Turning the display on
 *
 * The display buffer is allocated with a size of 1025 bytes (128x64 mono display + 1 byte for the initial instruction).
 * The buffer is initialized with 0x00, and the initial instruction byte is set to 0x40.
 *
 * Finally, the display buffer is written to the display.
 */
void SSD1306Oled::Begin(void) {
    // initialise I2C
    i2c_init(i2c_, 100 * 1000);
    gpio_set_function(sda_, GPIO_FUNC_I2C);
    gpio_set_function(scl_, GPIO_FUNC_I2C);
    gpio_pull_up(sda_);
    gpio_pull_up(scl_);

    // initialise the Display
    uint8_t init_sequence[] = {SET_DISP | 0x00,
                               SET_MEM_ADDR,
                               0x00,
                               SET_DISP_START_LINE | 0x00,
                               SET_SEG_REMAP | 0x01,
                               SET_MUX_RATIO,
                               height_ - 1,
                               SET_COM_OUT_DIR | 0x08,
                               SET_DISP_OFFSET,
                               0x00,
                               SET_DISP_CLK_DIV,
                               0x80,
                               SET_PRECHARGE,
                               0xF1,
                               SET_VCOM_DESEL,
                               0x40,
                               SET_CONTRAST,
                               0xFF,
                               SET_ENTIRE_ON,
                               SET_NORM_INV,
                               SET_CHARGE_PUMP,
                               0x14,
                               SET_DISP | 0x01};

    for (uint8_t i = 0; i < sizeof(init_sequence); i++) {
        WriteCommand(init_sequence[i]);
    }

    // set up the Display buffer
    display_buffer_size_ = 1024 + 1;  // 128x64 mono Display + byte for the initial instruction
    display_buffer_ = (uint8_t *)malloc(display_buffer_size_);

    display_buffer_[0] = 0x40;  // initial instruction to write out
    for (uint16_t i = 1; i < display_buffer_size_; i++) {
        display_buffer_[i] = 0b00000000;  // initial bit pattern on start up
    }

    Display();  // actually write the Display

    return;
}

// output the Display buffer
/**
 * @brief Displays the contents of the display buffer on the SSD1306 OLED screen.
 *
 * This function sends the necessary commands to the SSD1306 OLED controller to set the memory addressing mode,
 * column address range, and page address range. It then writes the contents of the display buffer to the OLED screen
 * using I2C communication.
 *
 * The display buffer should contain the pixel data to be displayed on the screen. The function assumes that the
 * display buffer size is appropriate for the screen resolution (128x64 pixels).
 *
 * @note This function uses the I2C interface to communicate with the SSD1306 OLED controller.
 */
void SSD1306Oled::Display(void) {
    WriteCommand(SET_MEM_ADDR);
    WriteCommand(0b01);

    WriteCommand(SET_COL_ADDR);
    WriteCommand(0);    // first column
    WriteCommand(127);  // last column

    WriteCommand(SET_PAGE_ADDR);
    WriteCommand(0);  // first page
    WriteCommand(7);  // last page

    i2c_write_blocking(i2c_, i2c_address_, display_buffer_, display_buffer_size_, false);

    return;
}

/**
 * @brief Clears the entire display buffer, turning all pixels off.
 *
 * This function iterates through the display buffer and sets each byte to 0x00,
 * effectively turning off all pixels on the OLED display.
 */
void SSD1306Oled::ClearAll(void) {
    for (uint16_t i = 1; i < display_buffer_size_; i++) {
        display_buffer_[i] = 0x00;  // all pixels off
    }
    return;
}

// returns value of a pixel
/**
 * @brief Get the value of a pixel at the specified coordinates.
 *
 * This function retrieves the value of a pixel from the display buffer.
 * It checks if the coordinates are within the valid range of the display.
 * If the coordinates are outside the screen area, it returns 0.
 *
 * @param X The x-coordinate of the pixel.
 * @param Y The y-coordinate of the pixel.
 * @return uint8_t The value of the pixel (1 if the pixel is set, 0 if it is not).
 */
* / uint8_t SSD1306Oled::SetPixel(int16_t X, int16_t Y, uint8_t V) {
    if (X < 0 || X > width_ || Y < 0 || Y >= height_ || X < clip_rect_.x || X > (clip_rect_.x + clip_rect_.w) ||
        Y < clip_rect_.y || Y > (clip_rect_.y + clip_rect_.h)) {
        // outside screen area
        return 0;
    }

    uint8_t page = Y / 8;
    uint8_t bit = 1 << (Y % 8);
    uint8_t *ptr = display_buffer_ + X * 8 + page + 1;  // +1 to skip the initial command byte

    uint8_t current_byte = *ptr;
    uint8_t current_bit = (current_byte >> (Y % 8)) & 0x01;

    switch (mode_) {
        case MODE_SET: {
            // do nothing
            break;
        }
        case MODE_OR: {
            // logical OR with exisiting value
            V |= GetPixel(X, Y);
            break;
        }
        case MODE_AND: {
            // logical AND with exisiting value
            V &= GetPixel(X, Y);
            break;
        }
        case MODE_XOR: {
            V ^= GetPixel(X, Y);
            break;
        }
        case MODE_NOT: {
            V = !V;
        }
    }

    if (V == 0) {
        *ptr &= ~bit;
    } else if (V == 1) {
        *ptr |= bit;
    }

    return current_bit;
}

/**
 * @brief Sends a command to the SSD1306 OLED display.
 *
 * This function sends a single command byte to the SSD1306 OLED display
 * over I2C. The command is sent with a control byte indicating that the
 * following byte is a command.
 *
 * @param cmd The command byte to send to the display.
 */
void SSD1306Oled::WriteCommand(uint8_t cmd) {
    uint8_t B[2];

    B[0] = 0x80;  // constant for sending a command
    B[1] = cmd;

    i2c_write_blocking(i2c_, i2c_address_, B, 2, false);
    return;
}
