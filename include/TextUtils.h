#pragma once
#include <Arduino.h>
#include <U8g2_for_Adafruit_GFX.h>

// Utilitaires d'affichage de texte avec U8g2 (aucun état : fonctions pures).
namespace textutils {

// Copie src dans out (taille n, au moins 4) en le raccourcissant avec "..."
// s'il dépasse maxW pixels avec la police actuellement active.
void fitText(U8G2_FOR_ADAFRUIT_GFX &g, const char *src, char *out, size_t n, int maxW);

// Affiche text sur au plus maxLines lignes, avec retour à la ligne par mots.
// y = ligne de base de la première ligne, lineH = interligne en pixels.
// Si le texte est coupé, la dernière ligne se termine par "...".
void drawWrapped(U8G2_FOR_ADAFRUIT_GFX &g, const char *text,
                 int x, int y, int lineH, int maxW, int maxLines);

}  // namespace textutils