#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <time.h>
#include "base/drawable.h"
#include "base/pageturner.h"
#include "primitives/pane.h"
#include "renderers/line_renderer_dda.h"
#include "application/simp_reader.h"

class Client : public PageTurner
{
public:
    Client(Drawable *drawable);
    void nextPage(QString filename);

private:
    Drawable *drawable;
    void draw_rect(int x1, int y1, int x2, int y2, uint color);
    Pane* panes[4];
};

#endif // CLIENT_H
