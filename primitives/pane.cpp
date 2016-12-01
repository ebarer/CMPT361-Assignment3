#include "pane.h"

Pane::Pane(int x_off, int y_off, Drawable* drawable) {
    this->base_x = x_off;
    this->base_y = y_off;
    this->drawable = drawable;
}

int Pane::getWidth() {
    return this->width;
}

int Pane::getHeight() {
    return this->height;
}

void Pane::setPixel(int x, int y, uint color) {
    if ((x >= 0 && x < width) && (y >= 0 && y < height)) {
        drawable->setPixel(base_x + x, base_y + y, color);
    }
}

uint Pane::getPixel(int x, int y) {
    return (uint)(drawable->getPixel(x + base_x, y + base_y));
}

void Pane::updateScreen() {
    drawable->updateScreen();
}

void Pane::setBackground(uint color) {
    for (int x = 0; x < this->width; x++) {
        for (int y = 0; y < this->height; y++) {
            setPixel(x, y, color);
        }
    }
}
