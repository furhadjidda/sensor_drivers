
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
#include "display_driver.hpp"
#include "font7x5.hpp"
SSD1306Oled Display(i2c0, 4, 5, 0x3c);

int main(void) {
    stdio_init_all();
    Display.Begin();
    Display.SetFont(&f7x5font);
    Display.DrawString(0, 0 - Display.GetFontHeight(Display.GetFont()), "Hello everyone Example", 1);
    Display.DrawHLine(5, 10, 118, 1);
    Display.DrawHLine(5, 13, 118, 1);
    Display.DrawString(0, 20, "for the pixel 2", 1);
    Display.DrawString(0, 10, "for the pixel 3", 1);
    // Display.DrawFilledCircle(40, 40, 20, 1);
    // Display.DrawFilledRectangle(20, 20, 60, 30, 1);
    Display.Display();

    return 0;
}