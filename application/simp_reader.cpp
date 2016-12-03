#include "simp_reader.h"

using namespace std;
bool debug = false;

void SimpReader::readFile(QString fileName, Pane* drawable) {
    ctmStack = QStack<QMatrix4x4>();

    // Define base CTM and CAM:
    ctm = QMatrix4x4();
    proj = QMatrix4x4();
    cam = QMatrix4x4();
    stm = QMatrix4x4();

    QFile inputFile(fileName);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        cout << "Couldn't read file" << endl;
        return;
    }

    QTextStream in(&inputFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        iter = line.constBegin();
        readWhitespace();

        // BLOCK STATEMENT
        if ((*iter).toLatin1() == '{') {
            ctmStack.push(ctm);
            continue;
        }
        if ((*iter).toLatin1() == '}') {
            ctm = ctmStack.pop();
            continue;
        }

        QString token = parseToken();

        // CAMERA + LIGHTING
        if (token == "camera") {
            camera = parseCamera(drawable);
            continue;
        }
        if (token == "light") {
            parseLight();
            continue;
        }

        // TRANSFORMATIONS
        if (token == "scale") {
            parseScale();
            continue;
        }
        if (token == "rotate") {
            parseRotate();
            continue;
        }
        if (token == "translate") {
            parseTranslate();
            continue;
        }

        // PRIMITIVES
        if (token == "line") {
            parseLine(drawable);
            continue;
        }
        if (token == "mesh") {
            parseMesh(drawable);
            continue;
        }
        if (token == "polygon") {
            parsePolygon(drawable);
            continue;
        }

        // RENDERING STYLE
        if (token == "phong") {
            if (debug) { cout << "[PHONG]" << endl; }
            style = Phong;
            continue;
        }
        if (token == "gouraud") {
            if (debug) { cout << "[GOURAUD]" << endl; }
            style = Gouraud;
            continue;
        }
        if (token == "flat") {
            if (debug) { cout << "[FLAT]" << endl; }
            style = Flat;
            continue;
        }

        // RENDER ATTRIBUTES
        if (token == "surface") {
            parseSurface();
            continue;
        }
        if (token == "ambient") {
            parseAmbient();
            continue;
        }
        if (token == "wire") {
            if (debug) { cout << "[WIREFRAME]" << endl; }
            wireframe = true;
            continue;
        }
        if (token == "filled") {
            if (debug) { cout << "[FILLED]" << endl; }
            wireframe = false;
            continue;
        }
    }

    inputFile.close();
}


// CAMERA + LIGHTING
bool SimpReader::parseCamera(Pane* drawable) {
    if (debug) { cout << "CAMERA: "; }

    float xlow = parseDigit();
    float ylow = parseDigit();
    float xhigh = parseDigit();
    float yhigh = parseDigit();
    float hither = parseDigit();
    float yon = parseDigit();

    cam = ctm.inverted();
    proj.frustum(xlow, xhigh, ylow, yhigh, -hither, -yon);
    buffer = ZBuffer(drawable->getHeight(), drawable->getWidth(), hither, yon);

    // Configure STM:   map to [0, 650] camera space
    float dX = xhigh - xlow;
    float dY = yhigh - ylow;
    float xMid = xlow + ((xhigh-xlow)/2);
    float yMid = ylow + ((yhigh-ylow)/2);

    float translateX = round((drawable->getWidth() / 2) - xMid);
    float translateY = round((drawable->getHeight() / 2) - yMid);
    stm.translate(translateX, translateY, 0);

    float scaleX = drawable->getWidth() / dX;
    float scaleY = drawable->getHeight() / dY;
    stm.scale(scaleX, -scaleY, 1);

    return true;
}

void SimpReader::parseLight() {
    //light <red> <green> <blue> <A> <B>
    float r = parseDigit();
    float g = parseDigit();
    float b = parseDigit();
    float attA = parseDigit();
    float attB = parseDigit();

    QVector3D v =  ctm * QVector3D(0, 0, 0);
    Point p = Point(v);
    lights.append(Light(p, r, g, b, attA, attB));
}


// TRANSFORMATIONS
void SimpReader::parseRotate() {
    if (debug) { cout << "ROTATE: "; }

    float x = 0, y = 0, z = 0;
    char axis = (*iter++).toLatin1();
    switch(axis) {
    case 'X':
        x = 1;
        break;
    case 'Y':
        y = 1;
        break;
    case 'Z':
        z = -1;
        break;
    }

    readWhitespace();

    float angle = parseDigit();

    if (debug) { cout << axis << ", " << angle << endl; }

    // Generate rotate matrix
    QMatrix4x4 matT = QMatrix4x4();
    matT.rotate(angle, x, y, z);

    // Store in CTM
    this->ctm = this->ctm * matT;
}

void SimpReader::parseScale() {
    if (debug) { cout << "SCALE: "; }

    float sx = parseDigit();
    float sy = parseDigit();
    float sz = parseDigit();

    if (debug) { cout << sx << ", " << sy << ", " << sz << endl; }

    // Generate scale matrix
    QMatrix4x4 matT = QMatrix4x4();
    matT.scale(sx, sy, sz);

    // Store in CTM
    this->ctm = this->ctm * matT;
}

void SimpReader::parseTranslate() {
    if (debug) { cout << "TRANSLATE: "; }

    float tx = parseDigit();
    float ty = parseDigit();
    float tz = parseDigit();
    if (debug) { cout << tx << ", " << ty << ", " << tz << endl; }

    // Generate transformation matrix
    QMatrix4x4 matT = QMatrix4x4();
    matT.translate(tx, ty, tz);

    // Store in CTM
    this->ctm = this->ctm * matT;
}


// PRIMITIVES
Point SimpReader::parsePoint() {
    float x, y, z, r, g, b;

    expect('(');
    x = parseDigit();
    readSeparator();
    y = parseDigit();
    readSeparator();
    z = parseDigit();

    // If separator found, RGB values included
    if (next() == ',') {
        expect(',');
        r = parseDigit();
        readSeparator();
        g = parseDigit();
        readSeparator();
        b = parseDigit();
    } else {
        r = surface.colors[RED];
        g = surface.colors[GREEN];
        b = surface.colors[BLUE];
    }

    expect(')');
    readWhitespace();

    QVector3D w = ctm * QVector3D(x, y, z);
    QVector3D c = cam * w;
    QVector3D s = stm * proj * c;
    s.setZ(w.z());

    QVector<float> color = QVector<float>(3);
    color[RED]   = r;
    color[GREEN] = g;
    color[BLUE]  = b;

    Point p = Point(s, c, w, color);

    // If 'n' found, NORMAL included
    if (next() == 'n' && !normalAveraging) {
        float nx, ny, nz;

        expect('n');

        readWhitespace();
        expect('[');

        nx = parseDigit();
        readSeparator();
        ny = parseDigit();
        readSeparator();
        nz = parseDigit();

        expect(']');
        readWhitespace();

        QVector3D n = ctm * QVector3D(nx, ny, nz); // NORMAL in World Space
        p.setNormal(n);
    }

    return p;
}

void SimpReader::parseLine(Pane* drawable) {
    if (debug) { cout << "<LINE>" << endl; }

    Point p1 = parsePoint();
    Point p2 = parsePoint();

    LineRendererDDA br = LineRendererDDA();
    Line line = Line(p1, p2);
    br.draw_line(line, drawable, this);
}

void SimpReader::parsePolygon(Pane* drawable) {
    if (debug) { cout << "<POLYGON>" << endl; }

    Point p1 = parsePoint();
    Point p2 = parsePoint();
    Point p3 = parsePoint();

    QVector3D normal = QVector3D::crossProduct((p2.getWorld() - p1.getWorld()), (p3.getWorld() - p1.getWorld())).normalized();

    p1.setNormal(normal);
    p2.setNormal(normal);
    p3.setNormal(normal);

    // Handle lighting
    QVector3D camera = cam.inverted() * QVector3D(0,0,0);
    QVector<float> intensity = LightingModel::calculateLighting(p1, surface, ambient, lights, camera);

    p1.setColor(intensity);
    p2.setColor(intensity);
    p3.setColor(intensity);

    PolygonRenderer pr = PolygonRenderer();
    Polygon poly = Polygon(p1, p2, p3);
    pr.draw_polygon(poly, drawable, this);
}

void SimpReader::parseMesh(Pane* drawable) {
    if (debug) { cout << "<MESH>" << endl; }

    expect('"');
    QString meshName = parseFilename();
    expect('"');

    if (debug) { cout << meshName.toUtf8().constData() << endl; }
    QFile meshFile(meshName);
    if (!meshFile.open(QIODevice::ReadOnly)) {
        cout << "Couldn't read mesh" << endl;
        return;
    }

    // Preserve iterator position and surface
    QString::const_iterator backupIter = iter;
    Surface backupSurface = surface;

    QTextStream in(&meshFile);
    QString line = in.readLine();
    iter = line.constBegin();
    readWhitespace();

    if (next() == 's' || next() == 'n') {
        QString token = parseToken();

        if (token == "surface") {
            parseSurface();
            token = parseToken();
        }

        if (token == "normal") {
            normalAveraging = true;
        }

        line = in.readLine();
    }

    // Get number of columns/rows for mesh
    int columns = line.toInt();
    line = in.readLine();
    int rows = line.toInt();

    // Define mesh matrix
    Point** points = new Point*[columns];

    // Retrieve all matrix points
    for (int r = 0; r < rows; r++) {
        points[r] = new Point[columns];

        for (int c = 0; c < columns; c++) {
            points[r][c] = parsePoint();
        }
    }

    // Render mesh surfaces
    int polyGenerated = 0;
    PolygonRenderer pr = PolygonRenderer();
    for (int r = 0; r < rows-1; r++) {
        for (int c = 0; c < columns-1; c++) {
            Point pa1 = points[r][c];
            Point pa2 = points[r+1][c];
            Point pa3 = points[r+1][c+1];
            Polygon poly1 = Polygon(pa1, pa2, pa3);

            Point pb1 = points[r][c];
            Point pb2 = points[r][c+1];
            Point pb3 = points[r+1][c+1];
            Polygon poly2 = Polygon(pb1, pb2, pb3);

            pr.draw_polygon(poly1, drawable, this);
            pr.draw_polygon(poly2, drawable, this);
            polyGenerated += 2;
        }
    }

    if (debug) { cout << polyGenerated << endl; }

    // Restore iterator and surface
    iter = backupIter;
    surface= backupSurface;
    normalAveraging = false;
}


// RENDER ATTRIBUTES
void SimpReader::parseSurface() {
    expect('(');

    surface.colors[RED] = parseDigit();
    readSeparator();
    surface.colors[GREEN] = parseDigit();
    readSeparator();
    surface.colors[BLUE] = parseDigit();

    expect(')');
    readWhitespace();

    surface.ks = parseDigit();
    readWhitespace();

    surface.alpha = parseDigit();
    readWhitespace();
}

void SimpReader::parseAmbient() {
    expect('(');

    ambient.colors[RED] = parseDigit();
    readSeparator();
    ambient.colors[GREEN] = parseDigit();
    readSeparator();
    ambient.colors[BLUE] = parseDigit();

    expect(')');
    readWhitespace();
}


// PARSERS
QString SimpReader::parseToken() {
    QString identifier = QString();

    readWhitespace();
    while((*iter).isLetter()) {
        identifier.append((*iter));
        iter++;
    }
    readWhitespace();

    return identifier;
}

float SimpReader::parseDigit() {
    QString param;

    readWhitespace();
    while((*iter).isDigit() || (*iter) == '+' || (*iter) == '-' || (*iter) == '.') {
        param.append((*iter++));
    }
    readWhitespace();

    return param.toFloat();
}

QString SimpReader::parseFilename() {
    QString filename;

    readWhitespace();
    while((*iter).toLatin1() != '"') {
        filename.push_back((*iter));
        iter++;
    }
    readWhitespace();

    return filename;
}


// HELPERS
char SimpReader::next() {
    readWhitespace();
    char test = (*iter).toLatin1();
    return test;
}
void SimpReader::expect(char c) {
    char test = (*iter).toLatin1();
    if (test != c) {
        cout << "Expecting " << c << ", found " << test << endl;
    }
    iter++;
}
void SimpReader::readWhitespace() {
    while((*iter).isSpace() || (*iter) == '\t') {
        iter++;
    }
}
void SimpReader::readSeparator() {
    while((*iter).isSpace() || (*iter) == ',') {
        iter++;
    }
}
