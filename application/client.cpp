#include "client.h"

Client::Client(Drawable *drawable) {
    this->drawable = drawable;
    panes[0] = new Pane(50, 50, this->drawable);
}

void Client::nextPage() {
    srand(time(NULL));

    Tests tester = Tests();
    SimpReader reader = SimpReader();
    LineRendererDDA br = LineRendererDDA();
    PolygonRenderer pr = PolygonRenderer();

    static int pageNumber = 0;
    std::cout << "PageNumber " << pageNumber << std::endl;

    switch(pageNumber % 8) {
    case 0: {
        draw_rect(0, 0, 750, 750, 0xffffffff); // Draw background
        panes[0]->setBackground(0xff000000);

        tester.renderPolygonGrid(true, true, &pr, panes[0]);

        drawable->updateScreen();
        break;
    }
    case 1: {
        draw_rect(0, 0, 750, 750, 0xffffffff); // Draw background
        panes[0]->setBackground(0xff000000);

        tester.renderPolygonGrid(true, false, &pr, panes[0]);

        drawable->updateScreen();
        break;
    }
    case 2: {
        draw_rect(0, 0, 750, 750, 0xffffffff); // Draw background
        panes[0]->setBackground(0xff000000);

        reader.setColors(0xffFFFFFF, 0xff000000);
        reader.readFile("page3/page3_1.simp", panes[0]);
        reader.readFile("page3/page3_2.simp", panes[0]);
        reader.readFile("page3/page3_3.simp", panes[0]);

        drawable->updateScreen();
        break;
    }
    case 3: {
        draw_rect(0, 0, 750, 750, 0xffffffff); // Draw background
        panes[0]->setBackground(0xff000000);

        reader.setColors(0xffff0000, 0xff0000ff);
        reader.readFile("page3/page3_1.simp", panes[0]);
        reader.readFile("page3/page3_2.simp", panes[0]);
        reader.readFile("page3/page3_3.simp", panes[0]);

        drawable->updateScreen();
        break;
    }
    case 4: {
        draw_rect(0, 0, 750, 750, 0xffffffff); // Draw background
        panes[0]->setBackground(0xff000000);

        reader.setColors(0xffFFFFFF, 0xff000000);
        reader.readFile("page5/page5.simp", panes[0]);

        drawable->updateScreen();
        break;
    }
    case 5: {
        draw_rect(0, 0, 750, 750, 0xffffffff); // Draw background
        panes[0]->setBackground(0xff000000);

        reader.setColors(0xffFFFFFF, 0xff000000);
        reader.readFile("test1.simp", panes[0]);

        drawable->updateScreen();
        break;
    }
    case 6: {
        draw_rect(0, 0, 750, 750, 0xffffffff); // Draw background
        panes[0]->setBackground(0xff000000);

        reader.setColors(0xffFFFFFF, 0xff000000);
        reader.readFile("test2.simp", panes[0]);

        drawable->updateScreen();
        break;
    }
    case 7: {
        draw_rect(0, 0, 750, 750, 0xffffffff); // Draw background
        panes[0]->setBackground(0xff000000);

        reader.setColors(0xffFFFFFF, 0xff000000);
        reader.readFile("test3.simp", panes[0]);

        drawable->updateScreen();
        break;
    }
    default: {
        draw_rect(0, 0, 750, 750, 0xffffffff); // Draw background
        panes[0]->setBackground(0xff000000);

        Line l1 = Line(Point(100,100,0,0xffff0000), Point(100,300,0,0xff0000ff));
        br.draw_line(l1, panes[0]);
        Line l2 = Line(Point(100,100,0,0xffff0000), Point(300,100,0,0xff00ff00));
        br.draw_line(l2, panes[0]);
        Line l3 = Line(Point(100,300,0,0xff0000ff), Point(300,300,0,0xffff0000));
        br.draw_line(l3, panes[0]);
        Line l4 = Line(Point(300,100,0,0xff00ff00), Point(300,300,0,0xffff0000));
        br.draw_line(l4, panes[0]);
        Line l5 = Line(Point(100,100,0,0xffff0000), Point(300,300,0,0xffff0000));
        br.draw_line(l5, panes[0]);
        Line l6 = Line(Point(300,100,0,0xff00ff00), Point(100,300,0,0xff0000ff));
        br.draw_line(l6, panes[0]);

        Polygon p0 = Polygon(Point(50,350,0,0xffff0000), Point(300,400,0,0xff00ff00), Point(200,600,0,0xff0000ff));
        pr.draw_polygon(p0, panes[0], true);

        Polygon p1 = Polygon(Point(350,350,0,0xffff0000), Point(600,400,0,0xff00ff00), Point(500,600,0,0xff0000ff));
        pr.draw_polygon(p1, panes[0], false);

        drawable->updateScreen();
        break;
    }
    }

    pageNumber += 1;
}

void Client::draw_rect(int x1, int y1, int x2, int y2, uint color) {
    for(int x = x1; x<x2; x++) {
        for(int y=y1; y<y2; y++) {
            drawable->setPixel(x, y, color);
        }
    }
}
