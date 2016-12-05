#include "shader_gouraud.h"

ShaderGouraud::ShaderGouraud() {}

void ShaderGouraud::shade(Point& v1, Point& v2, Point& v3) {
    if (v1.getNormal().isNull()) {
        QVector3D normal = QVector3D::crossProduct((v2.getWorld() - v1.getWorld()), (v3.getWorld() - v1.getWorld()));
        normal.normalize();

        v1.setNormal(normal);
        v2.setNormal(normal);
        v3.setNormal(normal);
    }
}
