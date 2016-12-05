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
#include "shaders/shade_basics.h"
#include "shaders/lighting_model.h"
#include "shaders/shader_flat.h"
#include "shaders/shader_gouraud.h"
#include "shaders/shader_phong.h"
#include "renderers/line_renderer_dda.h"
#include "renderers/polygon_renderer.h"

using namespace std;

enum RenderingStyle {
    Phong,
    Gouraud,
    Flat
};

class SimpReader
{
public:
    bool camera = false;
    bool normalAveraging = false;
    bool wireframe = false;
    RenderingStyle style = Phong;
    ZBuffer buffer;
    Surface surface = Surface();
    Ambient ambient = Ambient();
    QVector<Light> lights = QVector<Light>();

    QMatrix4x4 ctm;     // Object Space -> World Space
    QMatrix4x4 cam;     // World Space -> Camera Space
    QMatrix4x4 proj;    // Camera Space -> Projected Space
    QMatrix4x4 stm;     // Projected Space -> Screen Space
    QStack<QMatrix4x4> ctmStack;

    SimpReader() {}
    ~SimpReader() {}

    void readFile(QString fileName, Pane* drawable);

private:
    QString::const_iterator iter;

    // CAMERA + LIGHTING
    bool parseCamera(Pane* drawable);
    void parseLight();

    // TRANSFORMATIONS
    void parseRotate();
    void parseScale();
    void parseTranslate();

    // PRIMITIVES
    Point parsePoint();
    void parseLine(Pane* drawable);
    void parsePolygon(Pane* drawable);
    void parseMesh(Pane* drawable);

    // SHADERS
    void parsePhong();
    void parseGouraud();
    void parseFlat();

    // RENDER ATTRIBUTES
    void parseSurface();
    void parseAmbient();

    // PARSERS
    QString parseToken();
    float parseDigit();
    QString parseFilename();

    // HELPER
    char next();
    void expect(char c);
    void readWhitespace();
    void readSeparator();

    // PRINT HELPERS
    void printVector(QVector3D v) {
        cout << v.x() << "\t";
        cout << v.y() << "\t";
        cout << v.z() << endl;
    }
    void printMatrix(QMatrix4x4 m) {
        cout << endl;
        for (int i = 0; i < 4; i++) {
            cout << m.row(i).x() << "\t";
            cout << m.row(i).y() << "\t";
            cout << m.row(i).z() << "\t";
            cout << m.row(i).w() << endl;
        }
        cout << endl;
    }
};

#endif // SIMP_READER_H
