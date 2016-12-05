#ifndef SHADER_FLAT_H
#define SHADER_FLAT_H

#include "primitives/gfx_basics.h"

class ShaderFlat
{
public:
    ShaderFlat();

    static Point shade(Point& v1, Point& v2, Point& v3);
};

#endif // SHADER_FLAT_H
