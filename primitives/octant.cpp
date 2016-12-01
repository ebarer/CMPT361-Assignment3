#include "octant.h"

int determine_octant(Line line) {
    int dy = line.dy();
    int dx = line.dx();

    // Handle perfectly vertical/horizontal lines
    if (abs(dx) > abs(dy)) {
        if (dx > 0) {
            return (dy > 0) ? 1 : 8;
        } else {
            return (dy > 0) ? 4 : 5;
        }
    } else {
        if (dx > 0) {
            return (dy > 0) ? 2 : 7;
        } else {
            return (dy > 0) ? 3 : 6;
        }
    }
}

void convert(Point& p, int o) {
    switch(o) {
    case 1:
        break;
    case 2:
        swap(p);
        break;
    case 3:
        p.setX(-p.getX());
        swap(p);
        break;
    case 4:
        p.setX(-p.getX());
        break;
    case 5:
        p.setX(-p.getX());
        p.setY(-p.getY());
        break;
    case 6:
        p.setX(-p.getX());
        p.setY(-p.getY());
        swap(p);
        break;
    case 7:
        p.setY(-p.getY());
        swap(p);
        break;
    case 8:
        p.setY(-p.getY());
        break;
    default:
        break;
    }
}

void revert(Point& p, int o) {
    switch(o) {
    case 1:
        break;
    case 2:
        swap(p);
        break;
    case 3:
        swap(p);
        p.setX(-p.getX());
        break;
    case 4:
        p.setX(-p.getX());
        break;
    case 5:
        p.setX(-p.getX());
        p.setY(-p.getY());
        break;
    case 6:
        swap(p);
        p.setX(-p.getX());
        p.setY(-p.getY());
        break;
    case 7:
        swap(p);
        p.setY(-p.getY());
        break;
    case 8:
        p.setY(-p.getY());
        break;
    default:
        break;
    }
}

// Helpers
void negate(int& c) {
    c = -c;
}

void swap(Point& p) {
    int x = p.getX();
    int y = p.getY();

    p.setY(x);
    p.setX(y);
}
