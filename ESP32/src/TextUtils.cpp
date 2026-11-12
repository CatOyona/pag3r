#include "TextUtils.h"

namespace textutils {

void fitText(U8G2_FOR_ADAFRUIT_GFX &g, const char *src, char *out, size_t n, int maxW) {
    snprintf(out, n, "%s", src);
    if (g.getUTF8Width(out) <= maxW) return;

    size_t len = strlen(out);
    while (len > 0) {
        len--;
        while (len > 0 && (out[len] & 0xC0) == 0x80) len--;  // ne pas couper un accent en deux
        snprintf(out + len, n - len, "...");
        if (g.getUTF8Width(out) <= maxW) return;
    }
}

void drawWrapped(U8G2_FOR_ADAFRUIT_GFX &g, const char *text,
                 int x, int y, int lineH, int maxW, int maxLines) {
    char ln[64];
    const char *p = text;

    for (int row = 0; row < maxLines; row++) {
        while (*p == ' ') p++;
        if (!*p) break;

        // Plus long début de ligne qui tient dans maxW
        size_t k = 0, lastSpace = 0;
        bool overflow = false;
        while (p[k] && k < sizeof(ln) - 1) {
            ln[k] = p[k];
            ln[k + 1] = '\0';
            if (p[k] == ' ') lastSpace = k;
            // on ne mesure qu'à une frontière de caractère UTF-8
            if ((p[k + 1] & 0xC0) != 0x80 && g.getUTF8Width(ln) > maxW) { overflow = true; break; }
            k++;
        }

        size_t cut = k;
        if (overflow) {
            if (lastSpace > 0) {
                cut = lastSpace;                               // coupe au dernier espace
            } else {                                           // mot plus long que la ligne
                while (cut > 0 && (p[cut] & 0xC0) == 0x80) cut--;
                if (cut == 0) cut = 1;
            }
        }

        char out[72];
        snprintf(out, sizeof(out), "%.*s", (int)cut, p);
        p += cut;

        // Dernière ligne autorisée et il reste du texte : on l'indique avec "..."
        const char *rest = p;
        while (*rest == ' ') rest++;
        if (row == maxLines - 1 && *rest) {
            char tmp[72];
            snprintf(tmp, sizeof(tmp), "%s...", out);
            fitText(g, tmp, out, sizeof(out), maxW);
        }

        g.drawUTF8(x, y + row * lineH, out);
    }
}

}  // namespace textutils