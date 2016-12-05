#ifndef SHADER_GOURAUD_H
#define SHADER_GOURAUD_H

#include "primitives/gfx_basics.h"

class ShaderGouraud
{
public:
    ShaderGouraud();

    static void shade(Point& v1, Point& v2, Point& v3);
};

#endif // SHADER_GOURAUD_H
