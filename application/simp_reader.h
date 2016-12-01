#ifndef SIMP_READER_H
#define SIMP_READER_H

#include <stdlib.h>
#include <iostream>
#include <QString>
#include <QFile>
#include <QStack>
#include <QVector>
#include <QVector4D>
#include <QMatrix>
#include <QMatrix4x4>
#include "base/drawable.h"
#include "primitives/pane.h"
#include "primitives/gfx_basics.h"
#include "renderers/line_renderer.h"
#include "renderers/polygon_renderer.h"

using namespace std;

class SimpReader
{
public:
    SimpReader() {}
    ~SimpReader() {}

    void readFile(QString fileName, Drawable* drawable);

    // ACCESSORS
    void setColors(uint color1, uint color2) { colorNear = color1; colorFar = color2; }

private:
    QString::const_iterator iter;
    QMatrix4x4 ctm;
    QStack<QMatrix4x4> ctmStack;
    bool wireframe = false;
    uint colorNear = 0xffffffff;
    uint colorFar = 0xff000000;

    // TRANSFORMATIONS
    void parseRotate();
    void parseScale();
    void parseTranslate();

    // PRIMITIVES    
    void parseLine(Drawable* drawable);
    void parsePolygon(Drawable* drawable);
    void parseMesh(Drawable* drawable);

    // PARSERS
    void parseIdentifier();
    float parseDigit();
    QVector4D parsePoint();
    QVector4D parseMeshPoint();
    QString parseFilename();

    // HELPER
    void expect(char c);
    void readWhitespace();
    void readSeparator();
};

#endif // SIMP_READER_H
