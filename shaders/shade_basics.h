#ifndef SHADE_BASICS_H
#define SHADE_BASICS_H

#include "primitives/gfx_basics.h"

class Surface {
public:
    QVector<float> colors = QVector<float>(3);
    float ks;
    float alpha;

    Surface() {
        this->colors[RED]   = 1;
        this->colors[GREEN] = 1;
        this->colors[BLUE]  = 1;
        ks = 0.3;
        alpha = 8;
    }

    Surface(float r, float g, float b, float ks, float alpha) {
        this->colors[RED]   = r;
        this->colors[GREEN] = g;
        this->colors[BLUE]  = b;
        this->ks = ks;
        this->alpha = alpha;
    }
};

class Ambient {
public:
    QVector<float> colors = QVector<float>(3);

    Ambient() {
        this->colors[RED]   = 0;
        this->colors[GREEN] = 0;
        this->colors[BLUE]  = 0;
    }

    Ambient(float r, float g, float b) {
        this->colors[RED]   = r;
        this->colors[GREEN] = g;
        this->colors[BLUE]  = b;
    }
};

class Light {
public:
    Point p;
    QVector<float> colors = QVector<float>(3);
    float attA;
    float attB;

    Light() {}
    Light(Point p, float r, float g, float b, float attA, float attB) {
        this->p = p;

        this->colors[RED]   = r;
        this->colors[GREEN] = g;
        this->colors[BLUE]  = b;

        this->attA = attA;
        this->attB = attB;
    }
};

#endif // SHADE_BASICS_H
