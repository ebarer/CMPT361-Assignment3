#include "polygon_renderer.h"
#include "application/simp_reader.h"

using namespace std;

PolygonRenderer::PolygonRenderer() {}

void PolygonRenderer::draw_polygon(Polygon p, Drawable* drawable, SimpReader* simp) {
    if (simp->wireframe) {
        draw_wireframe(p, drawable, simp);
    } else {
        draw_shaded(p, drawable, simp);
    }
}

void PolygonRenderer::draw_wireframe(Polygon p, Drawable* drawable, SimpReader* simp) {
    Line l1 = Line(p.p1, p.p2);
    Line l2 = Line(p.p1, p.p3);
    Line l3 = Line(p.p2, p.p3);

    LineRendererDDA rdr = LineRendererDDA();
    rdr.draw_line(l1, drawable, simp);
    rdr.draw_line(l2, drawable, simp);
    rdr.draw_line(l3, drawable, simp);
}

QVector<int> lerpColor(QVector<int> c_1, QVector<int> c_2, int x, int x1, int x2) {
    int dx = x2-x1;

    if (dx == 0) { return c_1; }

    int b_1 = c_1[BLUE];
    int g_1 = c_1[GREEN];
    int r_1 = c_1[RED];

    int b_2 = c_2[BLUE];
    int g_2 = c_2[GREEN];
    int r_2 = c_2[RED];

    float q = (((float)x)/dx) + (-((float)x1)/dx);

    QVector<int> channels = QVector<int>(3);
    channels[BLUE]  = (int)(round(b_1 + (b_2-b_1)*q));
    channels[GREEN] = (int)(round(g_1 + (g_2-g_1)*q));
    channels[RED]   = (int)(round(r_1 + (r_2-r_1)*q));
    return channels;
}

QVector3D lerpWorld(QVector3D w1, QVector3D w2, int x, int x1, int x2) {
    int dx = x2-x1;
    if (dx == 0) { return w1; }
    float q = (((float)x)/dx) + (-((float)x1)/dx);

    QVector3D world = QVector3D();

    world.setX(w1.x() + (w2.x() - w1.x()) * q);
    world.setY(w1.y() + (w2.y() - w1.y()) * q);
    world.setZ(w1.z() + (w2.z() - w1.z()) * q);

    return world;
}

QVector3D lerpNormal(QVector3D n1, QVector3D n2, int x, int x1, int x2) {
    int dx = x2-x1;
    if (dx == 0) { return n1; }
    float q = (((float)x)/dx) + (-((float)x1)/dx);

    QVector3D normal = QVector3D();

    normal.setX(n1.x() + (n2.x() - n1.x()) * q);
    normal.setY(n1.y() + (n2.y() - n1.y()) * q);
    normal.setZ(n1.z() + (n2.z() - n1.z()) * q);

    return normal;
}

void PolygonRenderer::draw_shaded(Polygon p, Drawable* drawable, SimpReader* simp) {
    Line* lines[3];
    lines[0] = new Line(p.p1, p.p2);
    lines[1] = new Line(p.p1, p.p3);
    lines[2] = new Line(p.p2, p.p3);

    float maxLength = -1;
    int maxLine = -1;
    for (int i = 0; i < 3; i++) {
        if (lines[i]->dy() > maxLength) {
            maxLength = lines[i]->dy();
            maxLine = i;
        }
    }

    for (int i = 0; i < 3; i++) {
        if (i == maxLine) continue;

        for (int y = lines[i]->p1.getY(); y <= lines[i]->p2.getY(); y++) {
            int x1 = lines[maxLine]->p1.getX();

            if (lines[maxLine]->slope() != 0) {
                x1 = round((y - lines[maxLine]->intercept())/lines[maxLine]->slope());

            }

            int x2 = lines[i]->p1.getX();
            if (lines[i]->slope() != 0) {
                x2 = round((y - lines[i]->intercept())/lines[i]->slope());
            }

            QVector<int> c1 = lines[maxLine]->lerpColor(x1, y);
            QVector<int> c2 = lines[i]->lerpColor(x2, y);
            QVector3D w1 = lines[maxLine]->lerpWorld(x1, y);
            QVector3D w2 = lines[i]->lerpWorld(x2, y);
            QVector3D n1 = lines[maxLine]->lerpNormal(x1, y);
            QVector3D n2 = lines[i]->lerpNormal(x2, y);

            if (x1 > x2) {
                int xt = x1;
                x1 = x2;
                x2 = xt;
                QVector<int> ct = c1;
                c1 = c2;
                c2 = ct;
                QVector3D wt = w1;
                w1 = w2;
                w2 = wt;
                QVector3D nt = n1;
                n1 = n2;
                n2 = nt;
            }

            for (int x = x1; x < x2; x++) {
                QVector3D world = lerpWorld(w1, w2, x, x1, x2);
                Point po = Point(x, y, world.z(), lerpColor(c1, c2, x, x1, x2));
                po.setWorld(world);

                if (simp->buffer.update(po.getX(), po.getY(), po.getZ())) {
                    if (simp->style == Phong) {
                        po.setNormal(lerpNormal(n1, n2, x, x1, x2));

                        QVector3D camera = simp->cam.inverted() * QVector3D(0,0,0);
                        QVector<float> intensity = LightingModel::calculateLighting(po, simp->surface, simp->ambient, simp->lights, camera);

                        po.setColor(intensity);
                    }

                    drawable->setPixel(po.getX(), po.getY(), po.getColor());
                }
            }
        }
    }
}
