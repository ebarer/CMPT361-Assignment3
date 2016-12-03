#ifndef POLYGON_RENDERER_H
#define POLYGON_RENDERER_H

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "base/drawable.h"
#include "primitives/gfx_basics.h"
#include "primitives/octant.h"
#include "renderers/line_renderer_dda.h"

class SimpReader;

class PolygonRenderer
{
public:
    PolygonRenderer();
    void draw_polygon(Polygon p, Drawable* drawable, SimpReader* simp);
private:
    void draw_wireframe(Polygon p, Drawable* drawable, SimpReader* simp);
    void draw_shaded(Polygon p, Drawable* drawable, SimpReader* simp);
};

#endif // POLYGON_RENDERER_H
