#include "Screen.h"

void Screen::setFont(U8G2_FOR_ADAFRUIT_GFX &g, const uint8_t *font, uint16_t color) {
    g.setFont(font);
    g.setForegroundColor(color);
    g.setFontMode(1);
}

void Screen::drawIcon(GFXcanvas16 &c, int x, int y, const uint16_t *icon, int w, int h) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            const uint16_t px = icon[j * w + i];
            if (px != 0x0000) c.drawPixel(x + i, y + j, px);   // 0x0000 = transparent
        }
    }
}