#include "client.h"

Client::Client(Drawable *drawable) {
    this->drawable = drawable;
    panes[0] = new Pane(50, 50, this->drawable);
}

void Client::nextPage(QString filename) {
    SimpReader reader = SimpReader();

    srand(time(NULL));
    static int pageNumber = 0;
    std::cout << "PageNumber " << pageNumber++ << std::endl;

    draw_rect(0, 0, 750, 750, 0xffffffff); // Draw background
    panes[0]->setBackground(0xff000000);
    reader.readFile(filename, (Pane*)panes[0]);
    drawable->updateScreen();
}

void Client::draw_rect(int x1, int y1, int x2, int y2, uint color) {
    for(int x = x1; x<x2; x++) {
        for(int y=y1; y<y2; y++) {
            drawable->setPixel(x, y, color);
        }
    }
}
