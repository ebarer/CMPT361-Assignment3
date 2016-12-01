#ifndef GFX_BASICS_H
#define GFX_BASICS_H

#include <iostream>
#include <QVector>
#include <QVector4D>

using namespace std;

#define BLUE    0
#define GREEN   1
#define RED     2

class Point {
public:
    Point() : x(0), y(0), z(0) {
        splitColor(0xffffffff);
        depthCue();
    }
    Point(int x, int y, int z) : x(x), y(y), z(z) {
        splitColor(0xffffffff);
        depthCue();
    }
    Point(int x, int y, int z, uint color) : x(x), y(y), z(z) {
        splitColor(color);
        depthCue();
    }
    Point(int x, int y, int z, QVector<int> color) : x(x), y(y), z(z) {
        channels = color;
        depthCue();
    }
    Point(QVector4D v, uint color1, uint color2) {
        this->x = (int)round(v.x());
        this->y = (int)round(v.y());
        this->z = (int)round(v.z());
        depthCue(color1, color2);
    }

    void setX(int x) { this->x = x; }
    int getX() { return x; }
    void setY(int y) { this->y = y; }
    int getY() { return y; }
    void setZ(int z) { this->z = z; }
    int getZ() { return z; }

    void setColor(uint color) { splitColor(color); }
    uint getColor() { return mergeColor(); }
    QVector<int> getChannels() { return channels; }
    bool validColor() {
        if (channels[BLUE]  < 0 || channels[BLUE]  > 255) return false;
        if (channels[GREEN] < 0 || channels[GREEN] > 255) return false;
        if (channels[RED]   < 0 || channels[RED]   > 255) return false;
        return true;
    }

private:
    int x;
    int y;
    int z;
    float a;
    QVector<int> channels = QVector<int>(3);

    void splitColor(uint color) {
        channels[BLUE]  = (color & 0xff);
        channels[GREEN] = ((color >> 8) & 0xff);
        channels[RED]   = ((color >> 16) & 0xff);
    }
    uint mergeColor() {
        uint color = (0xff << 24) +
                     ((channels[RED]   & 0xff) << 16) +
                     ((channels[GREEN] & 0xff) << 8) +
                      (channels[BLUE]  & 0xff);
        return color;
    }

    void depthCue() {
        a = 1 - (this->z / 200.0);
        channels[BLUE]  = a * channels[BLUE];
        channels[GREEN] = a * channels[GREEN];
        channels[RED]   = a * channels[RED];
    }
    void depthCue(uint color1, uint color2) {
        int b_1 = (color1 & 0xff);
        int g_1 = ((color1 >> 8) & 0xff);
        int r_1 = ((color1 >> 16) & 0xff);

        int b_2 = (color2 & 0xff);
        int g_2 = ((color2 >> 8) & 0xff);
        int r_2 = ((color2 >> 16) & 0xff);

        a = 1 - (this->z / 200.0);
        channels[BLUE]  = (int)(round(b_2 + (b_1-b_2)*a));
        channels[GREEN] = (int)(round(g_2 + (g_1-g_2)*a));
        channels[RED]   = (int)(round(r_2 + (r_1-r_2)*a));
    }
};

class Line {
private:
    void swap_points() {
        Point pt = p1;
        p1 = p2;
        p2 = pt;
    }

public:
    Point p1;
    Point p2;

    Line(Point p1, Point p2) : p1(p1), p2(p2) {
        if (p1.getY() > p2.getY()) {
            this->swap_points();
        }
    }

    float dx() { return p2.getX() - p1.getX(); }
    float dy() { return p2.getY() - p1.getY(); }
    float dz() { return p2.getZ() - p1.getZ(); }

    float slope() {
        if (dx() == 0) return 0;
        return ((float)dy()) / dx();
    }

    float intercept() { return p1.getY() - (this->slope() * p1.getX()); }

    QVector<int> lerpColor(int x, int y) {
        QVector<int> c_1 = p1.getChannels();
        QVector<int> c_2 = p2.getChannels();

        int b_1 = c_1[BLUE];
        int g_1 = c_1[GREEN];
        int r_1 = c_1[RED];

        int b_2 = c_2[BLUE];
        int g_2 = c_2[GREEN];
        int r_2 = c_2[RED];

        // Determine which delta to use for interpolation
        // If either is 0, use the other, otherwise use largest
        float q = 0.0;
        if (dy() == 0) {
            q = (((float)x)/dx()) + (-p1.getX()/dx());
        } else if (dx() == 0) {
            q = (((float)y)/dy()) + (-p1.getY()/dy());
        } else {
             if (dy() >= dx()) {
                 q = (((float)y)/dy()) + (-p1.getY()/dy());
             } else {
                 q = (((float)x)/dx()) + (-p1.getX()/dx());
             }
        }

        QVector<int> channels = QVector<int>(3);
        channels[BLUE]  = (b_1 + (b_2-b_1)*q);
        channels[GREEN] = (g_1 + (g_2-g_1)*q);
        channels[RED]   = (r_1 + (r_2-r_1)*q);
        return channels;
    }
};

class Polygon {
private:
    void swap(Point& p1, Point& p2) {
        Point t = p1;
        p1 = p2;
        p2 = t;
    }

public:
    Point p1;
    Point p2;
    Point p3;

    Polygon(Point pa, Point pb, Point pc) : p1(pa), p2(pb), p3(pc) {
        // Make the largest dx = [x2 - x1] by setting
        // x1 = smallest x, x2 = largest x
        if (p2.getY() < p1.getY()) {
            swap(p1, p2);
        }

        if (p3.getY() < p1.getY()) {
            swap(p1, p3);
        }

        if (p3.getY() > p2.getY()) {
            swap(p2, p3);
        }
    }
};

#endif // GFX_BASICS_H
