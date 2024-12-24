
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

#ifndef PCA9685_H_
#define PCA9685_H_

#include <stdlib.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"

namespace pca9685 {

    class Pca9685Core{
        public:
            Pca9685Core( i2c_inst_t * aI2c, uint8_t aSda, uint8_t aScl, uint8_t aI2cAdd );

            void PwmInit();

            void SetPwmFrequency( unsigned int aFreq );

            void SetPwm( uint16_t aChannel, uint16_t aOn, uint16_t aOff );

            void SetServoPulse( uint16_t aChannel, uint16_t aPulse );

            void SetLevel( uint16_t aChannel, uint16_t aValue );

        private:
            void WriteRegister(uint8_t aReg, uint8_t aValue);
            uint8_t ReadRegister(uint8_t aReg);
            i2c_inst_t *mI2c;
            uint8_t mSda;
            uint8_t mScl;
            uint8_t mI2cAdd;

    };


}


#endif