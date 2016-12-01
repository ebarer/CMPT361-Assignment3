#ifndef POLYGON_RENDERER_H
#define POLYGON_RENDERER_H

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "base/drawable.h"
#include "primitives/gfx_basics.h"
#include "primitives/octant.h"
#include "renderers/line_renderer_dda.h"

class PolygonRenderer
{
public:
    PolygonRenderer();
    void draw_polygon(Polygon p, Drawable* drawable, bool wireframe = false);
private:
    void draw_wireframe(Polygon p, Drawable* drawable);
    void draw_shaded(Polygon p, Drawable* drawable);
};

#endif // POLYGON_RENDERER_H
