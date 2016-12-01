#include "line_renderer_dda.h"

using namespace std;

LineRendererDDA::LineRendererDDA() {}

void LineRendererDDA::draw_line(Line line, Drawable* drawable) {
    int octant = determine_octant(line);
    convert(line.p1, octant);
    convert(line.p2, octant);
    draw(line, octant, drawable);
}

void LineRendererDDA::draw(Line l, int octant, Drawable* drawable) {
    float y = l.p1.getY();

    for (int x = l.p1.getX(); x <= l.p2.getX(); x++) {
        QVector<int> color = l.lerpColor(x, round(y));
        Point p = Point(x, round(y), 0, color);
        revert(p, octant);

        if (p.validColor()) {
            drawable->setPixel(p.getX(), p.getY(), p.getColor());
        }

        y += l.slope();
    }
}
