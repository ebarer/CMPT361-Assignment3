#include "tests.h"

using namespace std;

// HELPERS
int random(int bound) { return rand() % bound; }
uint generateRandomColor() {
    uint color = (0xff << 24) +
                 ((random(COLOR_MAX) & 0xff) << 16) +
                 ((random(COLOR_MAX) & 0xff) << 8) +
                  (random(COLOR_MAX) & 0xff);

    return color;
}

// PAGE 1 + 2
void Tests::renderPolygonGrid(bool shift, bool wireframe, PolygonRenderer* renderer, Pane* drawable) {
    int numPoints = 10;
    int margin = 32;
    int shiftMin = 12;
    int shiftMax = 24;
    int step = (drawable->getWidth() - (2*margin)) / (numPoints-1);

    // Generate grid of points
    Point points[10][10];
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            int x = margin + i * step;
            int y = margin + j * step;
            int c = generateRandomColor();
            points[i][j] = Point(x, y, 0, c);
        }
    }

    // Shift pixels
    if (shift) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                int x = points[i][j].getX();
                int y = points[i][j].getY();

                int shiftX = (rand() % shiftMax) - shiftMin;
                int shiftY = (rand() % shiftMax) - shiftMin;

                points[i][j].setX(x + shiftX);
                points[i][j].setY(y + shiftY);
            }
        }
    }

    // Generate polygons
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            Point pa1 = points[i][j];
            Point pa2 = points[i+1][j];
            Point pa3 = points[i+1][j+1];
            Polygon poly1 = Polygon(pa1, pa2, pa3);

            Point pb1 = points[i][j];
            Point pb2 = points[i][j+1];
            Point pb3 = points[i+1][j+1];
            Polygon poly2 = Polygon(pb1, pb2, pb3);

            renderer->draw_polygon(poly1, drawable, wireframe);
            renderer->draw_polygon(poly2, drawable, wireframe);
        }
    }
}

// Page 3
