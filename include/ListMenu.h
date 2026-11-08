#pragma once
#include "Screen.h"

// Écran générique : liste verticale avec sélection et défilement.
// Sert pour les notifications comme pour les menus : le contenu des lignes
// est fourni par une fonction, et la cible de la sélection est configurable.
class ListMenu : public Screen {
public:
    // Écrit dans out (taille n) le texte de la ligne index.
    using LabelFn = void (*)(int index, char *out, size_t n);

    // label    : fonction qui donne le texte de chaque ligne
    // count    : nombre de lignes
    // onSelect : écran à ouvrir quand on valide une ligne (reçoit l'index en arg)
    ListMenu(LabelFn label, int count, ScreenId onSelect);

    void setCount(int count);   // si le nombre de lignes change (nouvelle notification...)

    void onEnter(int arg) override;
    Transition handleInput(Buttons &b) override;
    void draw(GFXcanvas16 &c, U8G2_FOR_ADAFRUIT_GFX &g) override;
    FooterHints footerHints() const override;

private:
    void select(int index);     // change la sélection, ajuste le défilement, demande un redessin

    LabelFn  _label;
    int      _count;
    ScreenId _onSelect;
    int      _sel   = 0;
    int      _first = 0;        // index de la première ligne visible
};