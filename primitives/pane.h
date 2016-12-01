#ifndef PANE_H
#define PANE_H

#include <QWidget>
#include <QVector>
#include "base/drawable.h"
#include "primitives/octant.h"

class Pane : public Drawable
{
public:
    Pane() {}
    Pane(int x_off, int y_off, Drawable* drawable);

    int getWidth();
    int getHeight();

    void setPixel(int x, int y, uint color);
    uint getPixel(int x, int y);

    void updateScreen();
    void setBackground(uint color);

private:
    int width = 650;
    int height = 650;
    int base_x;
    int base_y;
    Drawable* drawable;
};

#endif // PANE_H
