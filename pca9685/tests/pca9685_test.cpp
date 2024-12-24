
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
#include <stdio.h>

using namespace pca9685;

static const int SDIO = 20;
static const int SCLK = 21;
static const int freq = 100000;
static const uint8_t address = 0x40;

int main()
{
    stdio_init_all();
    Pca9685Core mObj(i2c0,SDIO,SCLK,address);
    // Initialize I2C
    mObj.PwmInit();

    mObj.SetPwmFrequency(50);
    printf(">>>==\n");
    // Test each channel one by one
    mObj.SetLevel(0, 1); 
    mObj.SetLevel(1, 0); 
    mObj.SetLevel(2, 1);
    /*
    To control your motor with the PCA9685 and the Waveshare Pico Motor Driver, 
    you typically need to set PWM values for three channels because the motor 
    driver uses an H-bridge configuration to control the motor. 
    The three channels are used as follows:

    PWM for Motor Speed (Enable Pin)
    PWM for Motor Direction (Two Direction Pins)

    In an H-bridge configuration, you have the following pins:

    Enable Pin: Controls the speed of the motor (PWM signal).
    Direction Pins: Two pins that determine the direction of the motor.

    To move the motor, you typically need to:

    Set the Enable Pin: Control the speed using a PWM signal.
    Set the Direction Pins: Determine the direction of rotation by setting one pin high and the other low, and vice versa for the opposite direction.
    */
    mObj.SetLevel(3, 1);
    mObj.SetLevel(4, 0);
    mObj.SetLevel(5, 1);

    while(1){

    }
    return 0;
}