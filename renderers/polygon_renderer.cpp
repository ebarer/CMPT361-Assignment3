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

float lerpZ(float z_1, float z_2, int x, int x1, int x2) {
    int dx = x2-x1;
    if (dx == 0) { return z_1; }
    float q = (((float)x)/dx) + (-((float)x1)/dx);
    return z_1 + ((z_2 - z_1) * q);
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
            float z1 = lines[maxLine]->lerpZ(x1, y);
            float z2 = lines[maxLine]->lerpZ(x2, y);

            if (x1 > x2) {
                int xt = x1;
                x1 = x2;
                x2 = xt;
                QVector<int> ct = c1;
                c1 = c2;
                c2 = ct;
                int zt = z1;
                z1 = z2;
                z2 = zt;
            }

            for (int x = x1; x <= x2; x++) {
                QVector<int> color = lerpColor(c1, c2, x, x1, x2);
                float z = lerpZ(z1, z2, x, x1, x2);
                Point po = Point(x, y, z, color);

                float zBuf = simp->buffer.get(x, y);
                if (simp->buffer.minValue <= z && simp->buffer.maxValue >= z) {
                    if (zBuf > z) {
                        simp->buffer.set(x, y, z);
                        drawable->setPixel(po.getX(), po.getY(), po.getColor());
                    }
                }
            }
        }
    }
}
