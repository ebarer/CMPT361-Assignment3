#ifndef LINE_RENDERER_DDA_H
#define LINE_RENDERER_DDA_H

#include "line_renderer.h"

class SimpReader;

class LineRendererDDA : public LineRenderer
{
public:
    LineRendererDDA();
    void draw_line(Line line, Drawable* drawable, SimpReader* simp);
private:
    void draw(Line line, int octant, Drawable* drawable, SimpReader* simp);
};

#endif // LINE_RENDERER_DDA_H
