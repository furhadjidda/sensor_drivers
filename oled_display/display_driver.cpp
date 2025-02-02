
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

#include "display_driver.hpp"

/**
 * @brief Retrieves the current mode of the display driver.
 *
 * @return uint8_t The current mode of the display driver.
 */
uint8_t DisplayDriver::GetMode(void) { return mode_; }

/**
 * @brief Sets the display mode.
 *
 * This function sets the display mode to the specified value and returns the new mode.
 *
 * @param Mode The mode to set.
 * @return The new mode that was set.
 */
uint8_t DisplayDriver::SetMode(uint8_t Mode) {
    mode_ = Mode;

    return mode_;
}

/**
 * @brief Draws a horizontal line on the display.
 *
 * This function draws a horizontal line starting from the specified (x, y)
 * coordinates and extending for the specified width (w). Each pixel in the
 * line is set to the specified value.
 *
 * @param x The x-coordinate of the starting point of the line.
 * @param y The y-coordinate of the starting point of the line.
 * @param w The width of the line in pixels.
 * @param Value The value to set for each pixel in the line.
 */
void DisplayDriver::DrawHLine(int16_t x, int16_t y, int16_t w, uint8_t Value) {
    for (uint16_t p = x; p <= x + w; p++) {
        SetPixel(p, y, Value);
    }
}

/**
 * @brief Draws a vertical line on the display.
 *
 * This function draws a vertical line starting from the coordinates (x, y)
 * and extending downwards for a height of h pixels. Each pixel in the line
 * is set to the specified value.
 *
 * @param x The x-coordinate of the starting point of the vertical line.
 * @param y The y-coordinate of the starting point of the vertical line.
 * @param h The height of the vertical line in pixels.
 * @param Value The value to set for each pixel in the vertical line.
 */
void DisplayDriver::DrawVLine(int16_t x, int16_t y, int16_t h, uint8_t Value) {
    for (uint16_t p = y; p <= y + h; p++) {
        SetPixel(x, p, Value);
    }
}

/**
 * @brief Draws a filled rectangle on the display.
 *
 * This function draws a filled rectangle starting from the top-left corner
 * (x, y) with the specified width (w) and height (h). The rectangle is filled
 * with the specified value.
 *
 * @param x The x-coordinate of the top-left corner of the rectangle.
 * @param y The y-coordinate of the top-left corner of the rectangle.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param Value The value to fill the rectangle with.
 */
void DisplayDriver::DrawFilledRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t Value) {
    for (uint16_t _y = y; _y <= y + h; _y++) {
        DrawHLine(x, _y, w, Value);
    }
}

/**
 * @brief Draws a rectangle on the display.
 *
 * This function draws a rectangle with the specified position, width, height, and value.
 * It uses horizontal and vertical lines to form the rectangle.
 *
 * @param x The x-coordinate of the top-left corner of the rectangle.
 * @param y The y-coordinate of the top-left corner of the rectangle.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param Value The value to set for the pixels of the rectangle (e.g., color or brightness).
 */
void DisplayDriver::DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t Value) {
    DrawHLine(x, y, w - 1, Value);
    DrawHLine(x, y + h - 1, w, Value);
    DrawVLine(x, y, h - 1, Value);
    DrawVLine(x + w - 1, y, h, Value);
}

/**
 * @brief Draws a line on the display from point (x0, y0) to point (x1, y1) with the specified value.
 *
 * This function uses Bresenham's line algorithm to draw a line between two points.
 *
 * @param x0 The x-coordinate of the starting point.
 * @param y0 The y-coordinate of the starting point.
 * @param x1 The x-coordinate of the ending point.
 * @param y1 The y-coordinate of the ending point.
 * @param Value The value to set for the pixels along the line (e.g., color or brightness).
 */
void DisplayDriver::DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t Value) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;) {
        SetPixel(x0, y0, Value);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

// draw circle, centred on x,y with radius r
/**
 * @brief Draws a circle on the display.
 *
 * This function uses the midpoint circle algorithm to draw a circle with the specified
 * center coordinates (cx, cy) and radius (r) on the display. The circle is drawn using
 * the specified pixel value.
 *
 * @param cx The x-coordinate of the center of the circle.
 * @param cy The y-coordinate of the center of the circle.
 * @param r The radius of the circle.
 * @param Value The pixel value to be used for drawing the circle.
 */
void DisplayDriver::DrawCircle(int16_t cx, int16_t cy, int16_t r, uint8_t Value) {
    int16_t x = 0, y, p;

    y = r;
    p = 3 - (2 * r);

    while (x < y) {
        SetPixel(cx + x, cy + y, Value);
        SetPixel(cx - x, cy + y, Value);
        SetPixel(cx + x, cy - y, Value);
        SetPixel(cx - x, cy - y, Value);
        SetPixel(cx + y, cy + x, Value);
        SetPixel(cx - y, cy + x, Value);
        SetPixel(cx + y, cy - x, Value);
        SetPixel(cx - y, cy - x, Value);

        x++;
        if (p < 0) {
            p = p + 4 * x + 6;
        } else {
            y--;
            p = p + 4 * (x - y) + 10;
        }
        SetPixel(cx + x, cy + y, Value);
        SetPixel(cx - x, cy + y, Value);
        SetPixel(cx + x, cy - y, Value);
        SetPixel(cx - x, cy - y, Value);
        SetPixel(cx + y, cy + x, Value);
        SetPixel(cx - y, cy + x, Value);
        SetPixel(cx + y, cy - x, Value);
        SetPixel(cx - y, cy - x, Value);
    }
}

// draw circle, centred on x,y with radius r
/**
 * @brief Draws a filled circle on the display.
 *
 * This function uses the midpoint circle algorithm to draw a filled circle
 * with the specified center coordinates and radius.
 *
 * @param cx The x-coordinate of the center of the circle.
 * @param cy The y-coordinate of the center of the circle.
 * @param r The radius of the circle.
 * @param Value The value to set for the pixels within the circle.
 */
void DisplayDriver::DrawFilledCircle(int16_t cx, int16_t cy, int16_t r, uint8_t Value) {
    int16_t x = 0, y, p;

    y = r;
    p = 3 - (2 * r);

    while (x < y) {
        DrawLine(cx + x, cy + y, cx - x, cy + y, Value);
        DrawLine(cx + x, cy - y, cx - x, cy - y, Value);
        DrawLine(cx + y, cy + x, cx - y, cy + x, Value);
        DrawLine(cx + y, cy - x, cx - y, cy - x, Value);

        x++;
        if (p < 0) {
            p = p + 4 * x + 6;
        } else {
            y--;
            p = p + 4 * (x - y) + 10;
        }
        DrawLine(cx + x, cy + y, cx - x, cy + y, Value);
        DrawLine(cx + x, cy - y, cx - x, cy - y, Value);
        DrawLine(cx + y, cy + x, cx - y, cy + x, Value);
        DrawLine(cx + y, cy - x, cx - y, cy - x, Value);
    }
}

/**
 * @brief Retrieves the current font used by the display driver.
 *
 * @return GFXfont* Pointer to the current font.
 */
GFXfont *DisplayDriver::GetFont(void) { return current_font_; }

/**
 * @brief Sets the current font for the display driver.
 *
 * This function sets the current font to be used by the display driver and returns the previous font.
 *
 * @param font A pointer to the new font to be set.
 * @return A pointer to the previous font.
 */
GFXfont *DisplayDriver::SetFont(const GFXfont *font) {
    GFXfont *oldFont;

    current_font_ = (GFXfont *)font;

    return oldFont;
}

/**
 * @brief Draws a glyph (character) on the display at the specified coordinates.
 *
 * This function uses the current font to draw a single character on the display.
 * It checks if the current font is set and if the character is within the font's
 * range. If so, it retrieves the glyph data and draws the character pixel by pixel.
 *
 * @param x The x-coordinate where the glyph should be drawn.
 * @param y The y-coordinate where the glyph should be drawn.
 * @param c The ASCII character to be drawn.
 * @param Value The value to set for the pixels of the glyph (e.g., color or brightness).
 * @return The horizontal distance to advance the cursor after drawing the glyph.
 */
int16_t DisplayDriver::DrawGlyph(int16_t x, int16_t y, char c, uint8_t Value) {
    if (current_font_ == NULL) {
        // if we don't have a font specified then abort
        return 0;
    }

    // we have a font, next we need to make sure it holds the ASCII character specified
    if ((c >= current_font_->first) && (c <= current_font_->last)) {
        // the required character is present in the font
        // offset is the ASCII character, less the first character in the font
        GFXglyph *G = current_font_->glyph + c - current_font_->first;
        uint16_t baseOffset = G->bitmapOffset;  // offset into the font bitmap array
        uint8_t w = G->width;
        uint8_t h = G->height;
        int8_t xo = G->xOffset;
        int8_t yo = G->yOffset;
        uint8_t xx, yy, bits = 0, bit = 0;

        for (yy = 0; yy < h; yy++) {
            for (xx = 0; xx < w; xx++) {
                if (!(bit++ & 7)) {
                    bits = current_font_->bitmap[baseOffset++];
                }
                if (bits & 0x80) {
                    SetPixel(x + xo + xx, y + current_font_->yAdvance + yo + yy, Value);
                }
                bits <<= 1;
            }
        }

        return G->xAdvance;
    } else {
        return 0;
    }

    return 0;
}

/**
 * @brief Draws a string on the display at the specified coordinates.
 *
 * This function iterates through each character in the provided string and
 * draws it on the display using the DrawGlyph method. The x-coordinate is
 * incremented by the width of each character as it is drawn.
 *
 * @param x The x-coordinate where the string drawing starts.
 * @param y The y-coordinate where the string drawing starts.
 * @param c The null-terminated string to be drawn on the display.
 * @param Value The value used for drawing the glyphs (e.g., color or intensity).
 * @return The x-coordinate after the last character has been drawn.
 */
int16_t DisplayDriver::DrawString(int16_t x, int16_t y, const char c[], uint8_t Value) {
    uint8_t idx = 0;
    char C = c[idx];
    int16_t _x = x;

    while (C != '\0') {
        _x += DrawGlyph(_x, y, C, Value);
        idx++;
        C = c[idx];
    }

    return _x;
}

/**
 * @brief Retrieves the height of the specified font.
 *
 * This function returns the height of the given font by accessing its yAdvance property.
 * If the provided font pointer is NULL, the function returns 0.
 *
 * @param font Pointer to the GFXfont structure representing the font.
 * @return uint8_t The height of the font in pixels, or 0 if the font pointer is NULL.
 */
uint8_t DisplayDriver::GetFontHeight(const GFXfont *font) {
    if (font != NULL) {
        return font->yAdvance;
    }

    return 0;
}

/**
 * @brief Clears the clipping rectangle by resetting its dimensions to the full display size.
 *
 * This function sets the clipping rectangle's x and y coordinates to 0, and its width and height
 * to the maximum dimensions of the display (127x63).
 */
void DisplayDriver::ClearClip(void) {
    clip_rect_.x = 0;
    clip_rect_.y = 0;
    clip_rect_.w = 127;
    clip_rect_.h = 63;
}

/**
 * @brief Sets the clipping rectangle for the display.
 *
 * This function defines a rectangular area on the display where subsequent
 * drawing operations will be confined. The clipping rectangle is specified
 * by its top-left corner (x, y) and its width (w) and height (h).
 *
 * @param x The x-coordinate of the top-left corner of the clipping rectangle.
 * @param y The y-coordinate of the top-left corner of the clipping rectangle.
 * @param w The width of the clipping rectangle.
 * @param h The height of the clipping rectangle.
 */
void DisplayDriver::SetClip(int16_t x, int16_t y, uint8_t w, uint8_t h) {
    clip_rect_.x = x;
    clip_rect_.y = y;
    clip_rect_.w = w;
    clip_rect_.h = h;
}

/**
 * @brief Sets the clipping rectangle for the display driver.
 *
 * This function sets the clipping rectangle, which defines the area of the display
 * that will be affected by subsequent drawing operations. The clipping rectangle
 * is specified by the RECT structure passed as a parameter.
 *
 * @param R Pointer to a RECT structure that defines the clipping rectangle.
 *          The RECT structure should contain the x and y coordinates of the
 *          top-left corner, and the width (w) and height (h) of the rectangle.
 */
void DisplayDriver::SetClip(RECT *R) {
    clip_rect_.x = R->x;
    clip_rect_.y = R->y;
    clip_rect_.w = R->w;
    clip_rect_.h = R->h;
}

/**
 * @brief Retrieves the clipping rectangle of the display.
 *
 * This function copies the current clipping rectangle dimensions into the
 * provided RECT structure.
 *
 * @param R Pointer to a RECT structure where the clipping rectangle
 *          dimensions will be stored.
 */
void DisplayDriver::GetClip(RECT *R) {
    R->x = clip_rect_.x;
    R->y = clip_rect_.y;
    R->w = clip_rect_.w;
    R->h = clip_rect_.h;
    return;
}

/**
 * @brief Clears the display within the defined clipping rectangle.
 *
 * This function sets the display mode to MODE_SET, iterates over each pixel
 * within the clipping rectangle defined by `clip_rect_`, and sets each pixel
 * to 0 (off). After clearing the pixels, it restores the previous display mode.
 */
void DisplayDriver::Clear(void) {
    uint8_t OldMode = SetMode(MODE_SET);
    for (int16_t x = clip_rect_.x; x <= (clip_rect_.x + clip_rect_.w); x++) {
        for (int16_t y = clip_rect_.y; y <= (clip_rect_.y + clip_rect_.h); y++) {
            SetPixel(x, y, 0);
        }
    }
    SetMode(OldMode);
}
