
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

#ifndef DisplayDisplay_h
#define DisplayDisplay_h

#define MODE_SET 0  // set pixel directly to specified value
#define MODE_OR 1   // logical OR with pixel already there
#define MODE_AND 2  // logical AND with pixel already there
#define MODE_XOR 3  // logical XOR with pixel already there
#define MODE_NOT 4  // draw the logical inverse of whats specified

#include <stdlib.h>

#include "pico/stdlib.h"

#define PROGMEM  // defined as nothing, allows using Adafruit font files without editing them

// Adafruit font structures
/// Font data stored PER GLYPH
/**
 * @struct GFXglyph
 * @brief Structure that defines a single glyph (character) in a font.
 *
 * This structure contains information about the bitmap representation of a
 * single glyph, including its dimensions, offsets, and the position of the
 * bitmap data within the font's bitmap array.
 *
 * @var GFXglyph::bitmapOffset
 * Pointer into the font's bitmap array where the glyph's bitmap data starts.
 *
 * @var GFXglyph::width
 * The width of the glyph's bitmap in pixels.
 *
 * @var GFXglyph::height
 * The height of the glyph's bitmap in pixels.
 *
 * @var GFXglyph::xAdvance
 * The distance to advance the cursor (x axis) after rendering this glyph.
 *
 * @var GFXglyph::xOffset
 * The horizontal distance from the cursor position to the upper-left corner of the bitmap.
 *
 * @var GFXglyph::yOffset
 * The vertical distance from the cursor position to the upper-left corner of the bitmap.
 */
typedef struct {
    uint16_t bitmapOffset;  ///< Pointer into GFXfont->bitmap
    uint8_t width;          ///< Bitmap dimensions in pixels
    uint8_t height;         ///< Bitmap dimensions in pixels
    uint8_t xAdvance;       ///< Distance to advance cursor (x axis)
    int8_t xOffset;         ///< X dist from cursor pos to UL corner
    int8_t yOffset;         ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
/**
 * @struct GFXfont
 * @brief Structure that defines a font for use with a graphical Display.
 *
 * This structure holds the necessary information to render a font on a graphical Display.
 * It includes the bitmap data for the glyphs, an array of glyphs, and the ASCII extents.
 *
 * @var GFXfont::bitmap
 * Pointer to the concatenated glyph bitmaps.
 *
 * @var GFXfont::glyph
 * Pointer to the array of glyphs.
 *
 * @var GFXfont::first
 * The ASCII value of the first character in the font.
 *
 * @var GFXfont::last
 * The ASCII value of the last character in the font.
 *
 * @var GFXfont::yAdvance
 * The vertical distance to advance the cursor after rendering a character (newline distance).
 */
typedef struct {
    uint8_t *bitmap;   ///< Glyph bitmaps, concatenated
    GFXglyph *glyph;   ///< Glyph array
    uint16_t first;    ///< ASCII extents (first char)
    uint16_t last;     ///< ASCII extents (last char)
    uint8_t yAdvance;  ///< Newline distance (y axis)
} GFXfont;

// generic rectangle structure
/**
 * @struct RECT
 * @brief A structure to represent a rectangle with position and size.
 *
 * This structure defines a rectangle by its top-left corner coordinates (x, y)
 * and its width (w) and height (h).
 *
 * @var int16_t RECT::x
 * The x-coordinate of the top-left corner of the rectangle.
 *
 * @var int16_t RECT::y
 * The y-coordinate of the top-left corner of the rectangle.
 *
 * @var uint8_t RECT::w
 * The width of the rectangle.
 *
 * @var uint8_t RECT::h
 * The height of the rectangle.
 */
typedef struct {
    int16_t x;
    int16_t y;
    uint8_t w;
    uint8_t h;
} RECT;

class DisplayDriver {
   public:
    uint8_t GetMode(void);
    uint8_t SetMode(uint8_t Mode);

    // virtual functions derived classes must provide
    virtual uint8_t GetPixel(int16_t X, int16_t Y) = 0;             // returns value of a pixel
    virtual uint8_t SetPixel(int16_t X, int16_t Y, uint8_t V) = 0;  // sets a pixel, returns previous value
    virtual void Begin(void) = 0;
    virtual void Display(void) = 0;
    virtual void ClearAll(void) = 0;

    // Clear the Display to pixel values of 0, respecting the clipping rectangle set
    void Clear(void);

    // clipping functions, Display maintains a clipping rectangle that can be adjusted and will void drawing operations
    // outside this rectangle
    void ClearClip(void);  // reset clip to the default screen size
    void SetClip(int16_t x, int16_t y, uint8_t w, uint8_t h);
    void SetClip(RECT *R);
    void GetClip(RECT *R);

    // generic drawing functions, all assume a single pixel wide pen
    void DrawHLine(int16_t x, int16_t y, int16_t w, uint8_t Value);
    void DrawVLine(int16_t x, int16_t y, int16_t h, uint8_t Value);
    void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t Value);
    void DrawFilledRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t Value);
    void DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t Value);
    void DrawCircle(int16_t x, int16_t y, int16_t r, uint8_t Value);
    void DrawFilledCircle(int16_t x, int16_t y, int16_t r, uint8_t Value);

    // text functions
    GFXfont *GetFont(void);
    GFXfont *SetFont(const GFXfont *font);
    uint8_t GetFontHeight(const GFXfont *font);
    int16_t DrawGlyph(int16_t x, int16_t y, char c, uint8_t Value);
    int16_t DrawString(int16_t x, int16_t y, const char c[], uint8_t Value);

   protected:
    uint8_t mode_ = MODE_SET;

    RECT clip_rect_ = {0, 0, 127, 63};

    GFXfont *current_font_ = NULL;
};

#endif
