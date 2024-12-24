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

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <ctype.h>
#include <stdlib.h>
#ifndef BUILD_FOR_HOST
#include <stdio.h>

#include "pico/stdlib.h"
#else
#include <stdint.h>

#include <cstdio>

#include "mock_hardware.hpp"
#endif

typedef uint8_t byte;

#define RAD_TO_DEG 57.29578

static uint64_t millis() { return time_us_64() / 1000; }

/*!
    @brief Checks whether a string starts with a specified prefix
    @param str Pointer to a string
    @param prefix Pointer to the prefix
    @return True if str starts with prefix, false otherwise
*/
static bool StrStartsWith(const char *str, const char *prefix) {
    while (*prefix) {
        if (*prefix++ != *str++) return false;
    }
    return true;
}

// Define the min template function
template <typename T>
T min(T a, T b) {
    return (a < b) ? a : b;
}

// Define the max template function
template <typename T>
T max(T a, T b) {
    return (a > b) ? a : b;
}

#endif