#include "line_renderer_dda.h"
#include "application/simp_reader.h"

using namespace std;

LineRendererDDA::LineRendererDDA() {}

void LineRendererDDA::draw_line(Line line, Drawable* drawable, SimpReader* simp) {
    int octant = determine_octant(line);
    convert(line.p1, octant);
    convert(line.p2, octant);
    draw(line, octant, drawable, simp);
}

void LineRendererDDA::draw(Line l, int octant, Drawable* drawable, SimpReader* simp) {
    float y = l.p1.getY();

    for (int x = l.p1.getX(); x <= l.p2.getX(); x++) {
        QVector<int> color = l.lerpColor(x, round(y));
        QVector3D world = l.lerpWorld(x, round(y));

        Point p = Point(x, round(y), 0, color);
        revert(p, octant);

        if (simp->buffer.update(p.getX(), p.getY(), world.z())) {
            drawable->setPixel(p.getX(), p.getY(), p.getColor());
        }

        y += l.slope();
    }
}
