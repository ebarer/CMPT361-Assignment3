#include "shader_flat.h"

ShaderFlat::ShaderFlat() {}

Point ShaderFlat::shade(Point& v1, Point& v2, Point& v3) {

    QVector3D loc = (v1.getWorld() + v2.getWorld() + v3.getWorld()) / 3.0;

    QVector<int> color = QVector<int>(3);
    for (int c = 0; c < 3; c++) {
        float red = v1.getChannels()[c] + v2.getChannels()[c] + v3.getChannels()[c];
        color[c] = round(red / 3.0);
    }

    QVector3D normal = QVector3D();
    // If NORMAL's are null, get face average
    if (v1.getNormal().isNull()) {
        normal = QVector3D::crossProduct((v2.getWorld() - v1.getWorld()), (v3.getWorld() - v1.getWorld()));
        normal.normalize();
    } else {
        normal = ((v1.getNormal() + v2.getNormal() + v3.getNormal()) / 3.0);
        normal.normalize();
    }

    Point center = Point(loc, normal, color);
    return center;
}
