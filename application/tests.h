#ifndef TESTS
#define TESTS

#include <stdlib.h>
#include <math.h>
#include "primitives/pane.h"
#include "primitives/octant.h"
#include "primitives/gfx_basics.h"
#include "renderers/line_renderer.h"
#include "renderers/polygon_renderer.h"

#define PANE_DIM 650
#define COLOR_MAX 256

class Tests
{
public:
    Tests() {}

    void renderPolygonStarburst(int numLines, int length, PolygonRenderer& renderer, Pane* drawable);
    void renderPolygonGrid(bool shift, bool wireframe, PolygonRenderer* renderer, Pane* drawable);
};

#endif // TESTS
