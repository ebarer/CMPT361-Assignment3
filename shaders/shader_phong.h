#ifndef SHADER_PHONG_H
#define SHADER_PHONG_H

#include "primitives/gfx_basics.h"

class ShaderPhong
{
public:
    ShaderPhong();

    static void shade(Point& v1, Point& v2, Point& v3);
};

#endif // SHADER_PHONG_H
