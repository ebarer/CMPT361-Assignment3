#ifndef LINE_RENDERER_DDA_H
#define LINE_RENDERER_DDA_H

#include "line_renderer.h"

class LineRendererDDA : public LineRenderer
{
public:
    LineRendererDDA();
    void draw_line(Line line, Drawable* drawable);
private:
    void draw(Line line, int octant, Drawable* drawable);
};

#endif // LINE_RENDERER_DDA_H
