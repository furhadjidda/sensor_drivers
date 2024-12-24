
/*
 *   This file is part of embedded software pico playground project.
 *
 *   embedded software pico playground projec is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   embedded software pico playground project is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License v3.0
 *   along with embedded software pico playground project.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "pca9685.hpp"
#include <cmath>

static const uint8_t __SUBADR1 = 0x02;
static const uint8_t __SUBADR2 = 0x03;
static const uint8_t __SUBADR3 = 0x04;
static const uint8_t __MODE1 = 0x00;
static const uint8_t __PRESCALE = 0xFE;
static const uint8_t __LED0_ON_L = 0x06;
static const uint8_t __LED0_ON_H = 0x07;
static const uint8_t __LED0_OFF_L = 0x08;
static const uint8_t __LED0_OFF_H = 0x09;
static const uint8_t __ALLLED_ON_L = 0xFA;
static const uint8_t __ALLLED_ON_H = 0xFB;
static const uint8_t __ALLLED_OFF_L = 0xFC;
static const uint8_t __ALLLED_OFF_H = 0xFD;
static const uint8_t WriteBufferSize = 2;

static const int SDIO = 20;
static const int SCLK = 21;
static const int freq = 100000;
static const uint8_t address = 0x40;

namespace pca9685 {

    Pca9685Core::Pca9685Core( i2c_inst_t * aI2c, uint8_t aSda, uint8_t aScl, uint8_t aI2cAdd )
    {
        mI2c = aI2c;
        mSda = aSda;
        mScl = aScl;
        mI2cAdd = aI2cAdd;
    }

    void Pca9685Core::PwmInit()
    {
        // initialise I2C
        i2c_init(mI2c, freq);
        gpio_set_function(mSda, GPIO_FUNC_I2C);
        gpio_set_function(mScl, GPIO_FUNC_I2C);
        gpio_pull_up(mSda);
        gpio_pull_up(mScl);
        
        // configure the PCA9685 for driving servos
        WriteRegister(__MODE1, 0x06);
	
    }

    void Pca9685Core::SetPwmFrequency( unsigned int aFreq )
    {
        // Sets the PWM frequency
        uint8_t oldmode = 0x00;
        // TODO: Add Debug statements

        uint8_t prescale = (25000000 / (4096 * aFreq)) - 1;
        // TODO: Add Debug statements

        // Read I2C
        WriteRegister(__MODE1,0x00);

        oldmode = ReadRegister(__MODE1);

        // Serial.println("oldmode = " + String(oldmode));
        // TODO: Add Debug statements

        uint8_t newmode = (oldmode & 0x7F) | 0x10; // sleep
        WriteRegister(__MODE1, newmode);

        WriteRegister(__PRESCALE, prescale);

        WriteRegister(__MODE1, oldmode);

        sleep_ms(500);

        WriteRegister(__MODE1, oldmode | 0x80);
    }

    void Pca9685Core::SetPwm( uint16_t aChannel, uint16_t aOn, uint16_t aOff )
    {
        // Sets a single PWM channel
        /*
        The PCA9685 expects separate register writes for the ON and OFF values,
        which the working SetPwm function achieves by writing to each register 
        individually.
        */
        WriteRegister(__LED0_ON_L + 4 * aChannel, aOn & 0xFF);
        WriteRegister(__LED0_ON_H + 4 * aChannel, aOn >> 8);

        WriteRegister(__LED0_OFF_L + 4 * aChannel, aOff & 0xFF);
        WriteRegister(__LED0_OFF_H + 4 * aChannel, aOff >> 8);
        //Serial.println("Channel " + String(aChannel) + " LED_ON " + String(aOn) + " LED_OFF " + String(aOff));
        // TODO: Add Debug statements
    }

    void Pca9685Core::SetServoPulse( uint16_t aChannel, uint16_t aPulse )
    {
        unsigned int pulse = aPulse * (4096 / 100);
        SetPwm(aChannel, 0, pulse);
    }

    void Pca9685Core::SetLevel( uint16_t aChannel, uint16_t aValue )
    {
        if (aValue == 1) {
            SetPwm(aChannel, 0, 4095);
        } else {
            SetPwm(aChannel, 0, 0);
        }
    }

    void Pca9685Core::WriteRegister(uint8_t aReg, uint8_t aValue)
    {
        uint8_t data[WriteBufferSize];
        
        data[0] = aReg;
        data[1] = aValue;
        
        i2c_write_blocking(mI2c, mI2cAdd, data, WriteBufferSize, false);
    }

    uint8_t Pca9685Core::ReadRegister(uint8_t aReg)
    {
        uint8_t data = 0x00;        
        i2c_write_blocking(mI2c, mI2cAdd, &aReg, 1, true);	// write register
        i2c_read_blocking(mI2c, mI2cAdd, &data, 1, false);		// read value
        return data;
    }

}

