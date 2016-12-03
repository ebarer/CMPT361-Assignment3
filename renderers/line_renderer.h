#ifndef LINE_RENDERER_H
#define LINE_RENDERER_H

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "base/drawable.h"
#include "primitives/octant.h"
#include "primitives/gfx_basics.h"

class SimpReader;

class LineRenderer
{
public:
    virtual void draw_line(Line line, Drawable* drawable, SimpReader* simp) = 0;
};

#endif // LINE_RENDERER_H
