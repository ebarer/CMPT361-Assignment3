#ifndef GFX_BASICS_H
#define GFX_BASICS_H

#include "math.h"
#include <iostream>
#include <QVector>
#include <QVector3D>

using namespace std;

enum COLOR_INDEX {
    RED,
    GREEN,
    BLUE
};

class Point {
private:
    int x;
    int y;
    float z;

    QVector<int> channels = QVector<int>(3);
    QVector3D world  = QVector3D();
    QVector3D normal = QVector3D();

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
public:
    Point() : x(0), y(0), z(0) {
        splitColor(0xffffffff);
    }
    Point(int x, int y, float z) : x(x), y(y), z(z) {
        splitColor(0xffffffff);
    }
    Point(int x, int y, float z, uint color) : x(x), y(y), z(z) {
        splitColor(color);
    }
    Point(int x, int y, float z, QVector<int> color) : x(x), y(y), z(z) {
        this->channels = color;
    }
    Point(QVector3D v) {
        this->x = (int)round(v.x());
        this->y = (int)round(v.y());
        this->z = v.z();
        this->world = v;
    }
    Point(QVector3D s, QVector3D w, QVector<float> color) {
        this->x = (int)round(s.x());
        this->y = (int)round(s.y());
        this->z = s.z();

        this->world = w;

        this->setColor(color);
    }
    // Lighting constructor
    Point(QVector3D world, QVector3D normal, QVector<int> color) {
        this->world = world;
        this->normal = normal;
        this->channels = color;
    }

    void setX(int x) { this->x = x; }
    int getX() { return x; }
    void setY(int y) { this->y = y; }
    int getY() { return y; }
    void setZ(float z) { this->z = z; }
    float getZ() { return z; }
    QVector3D getLoc() {
        return QVector3D(this->x, this->y, this->z);
    }

    void setWorld(QVector3D w) { this->world = w; }
    QVector3D getWorld() { return this->world; }
    void setNormal(QVector3D n) { this->normal = n; }
    QVector3D getNormal() { return this->normal; }
    void addNormal(QVector3D n) { this->normal.operator +=(n); }
    void normalizeNormal(int count) {
        this->normal.operator /= (count);
        this->normal.normalize();
    }

    void setColor(QVector<float> color) {
        this->channels[RED]   = (int)round(255 * color[RED]);
        this->channels[GREEN] = (int)round(255 * color[GREEN]);
        this->channels[BLUE]  = (int)round(255 * color[BLUE]);
    }
    void setColor(uint color) { splitColor(color); }
    uint getColor() { return mergeColor(); }
    QVector<int> getChannels() { return channels; }
    bool validColor() {
        if (channels[BLUE]  < 0 || channels[BLUE]  > 255) return false;
        if (channels[GREEN] < 0 || channels[GREEN] > 255) return false;
        if (channels[RED]   < 0 || channels[RED]   > 255) return false;
        return true;
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

        int r_1 = c_1[RED];
        int g_1 = c_1[GREEN];
        int b_1 = c_1[BLUE];

        int r_2 = c_2[RED];
        int g_2 = c_2[GREEN];
        int b_2 = c_2[BLUE];

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
        channels[RED]   = (r_1 + (r_2-r_1)*q);
        channels[GREEN] = (g_1 + (g_2-g_1)*q);
        channels[BLUE]  = (b_1 + (b_2-b_1)*q);
        return channels;
    }

    QVector3D lerpWorld(int x, int y) {
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

        QVector3D world = QVector3D();

        world.setX(p1.getWorld().x() + (p2.getWorld().x() - p1.getWorld().x()) * q);
        world.setY(p1.getWorld().y() + (p2.getWorld().y() - p1.getWorld().y()) * q);
        world.setZ(p1.getWorld().z() + (p2.getWorld().z() - p1.getWorld().z()) * q);

        return world;
    }

    QVector3D lerpNormal(int x, int y) {
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

        QVector3D normal = QVector3D();

        normal.setX(p1.getNormal().x() + (p2.getNormal().x() - p1.getNormal().x()) * q);
        normal.setY(p1.getNormal().y() + (p2.getNormal().y() - p1.getNormal().y()) * q);
        normal.setZ(p1.getNormal().z() + (p2.getNormal().z() - p1.getNormal().z()) * q);

        return normal;
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

class ZBuffer {
public:
    vector<vector<float>> buffer;
    int width;
    int height;
    float minValue;
    float maxValue;

    ZBuffer() : buffer(0, vector<float>(0, 0)) {}

    ZBuffer(int h, int w, float minValue, float maxValue) {
        initBuffer(h, w, minValue, maxValue);
    }

    void initBuffer(int h, int w, float minV, float maxV) {
        this->width = w + 1;
        this->height = h + 1;
        this->minValue = minV;
        this->maxValue = maxV;

        buffer.resize(width, vector<float>(height, maxValue + 1));
    }

    void set(int x, int y, float z) {
        if (x >= 0 && x < width) {
            if (y >= 0 && y < height) {
                buffer[x][y] = z;
            }
        }
    }

    float get(int x, int y) {
        if (x >= 0 && x < width) {
            if (y >= 0 && y < height) {
                return buffer[x][y];
            }
        }

        return maxValue;
    }

    bool update(int x, int y, float z) {
        if ((x >= 0 && x < width) && (y >= 0 && y < height)) {
            if (z >= minValue && z <= maxValue && z < buffer[x][y]) {
                buffer[x][y] = z;
                return true;
            }
        }

        return false;
    }
};

#endif // GFX_BASICS_H
