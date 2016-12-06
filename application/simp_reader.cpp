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
    buffer = ZBuffer(drawable->getHeight(), drawable->getWidth(), hither, yon);

    // Define frustum
    float width = (xhigh - xlow);
    float height = (yhigh - ylow);
    float depth = (yon - hither);

    proj.setRow(0, QVector4D(((2 * hither) / width), 0, ((xhigh + xlow) / width), 0));
    proj.setRow(1, QVector4D(0, ((2 * hither) / height), ((yhigh + ylow) / height), 0));
    proj.setRow(2, QVector4D(0, 0, ((yon+hither) / depth), ((2 * hither) / depth)));
    proj.setRow(3, QVector4D(0, 0, 1, 0));

    // Configure STM:   map to [0, 650] camera space
    float xMid = xlow + (width / 2);
    float yMid = ylow + (height /2);

    float translateX = round((drawable->getWidth() / 2) - xMid);
    float translateY = round((drawable->getHeight() / 2) - yMid);
    stm.translate(translateX, translateY, 0);

    float scaleX = drawable->getWidth() / 2;
    float scaleY = drawable->getHeight() / 2;
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
        x = -1;
        break;
    case 'Y':
        y = -1;
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
    QVector3D s = proj * c;
    s = stm * s;
    s.setZ(w.z());

    QVector<float> color = QVector<float>(3);
    color[RED]   = r;
    color[GREEN] = g;
    color[BLUE]  = b;

    Point p = Point(s, w, color);

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

    if (p1.getZ() > 50 || p1.getZ() < 1) {
        cout << p1.getZ() << endl;
    }

    if (p2.getZ() > 50 || p2.getZ() < 1) {
        cout << p2.getZ() << endl;
    }

    LineRendererDDA dda = LineRendererDDA();
    Line line = Line(p1, p2);
    dda.draw_line(line, drawable, this);
}

void SimpReader::parsePolygon(Pane* drawable) {
    if (debug) { cout << "<POLYGON>" << endl; }

    Point p1 = parsePoint();
    Point p2 = parsePoint();
    Point p3 = parsePoint();

    if (!wireframe) {
        QVector3D camera = cam.inverted() * QVector3D(0,0,0);

        if (style == Flat) {
            Point center = ShaderFlat::shade(p1, p2, p3);
            QVector<float> intensity = LightingModel::calculateLighting(center, surface, ambient, lights, camera);
            p1.setColor(intensity);
            p2.setColor(intensity);
            p3.setColor(intensity);
        }

        if (style == Gouraud) {
            ShaderGouraud::shade(p1, p2, p3);
            p1.setColor(LightingModel::calculateLighting(p1, surface, ambient, lights, camera));
            p2.setColor(LightingModel::calculateLighting(p2, surface, ambient, lights, camera));
            p3.setColor(LightingModel::calculateLighting(p3, surface, ambient, lights, camera));
        }

        if (style == Phong) {
            ShaderPhong::shade(p1, p2, p3);
        }
    }

    Polygon poly = Polygon(p1, p2, p3);
    PolygonRenderer pr = PolygonRenderer();
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
            line = in.readLine();
            iter = line.constBegin();
            token = parseToken();
        }

        if (token == "normal") {
            normalAveraging = true;
            line = in.readLine();
            iter = line.constBegin();
        }
    }

    // Get number of columns/rows for mesh
    int columns = line.toInt();
    line = in.readLine();
    int rows = line.toInt();

    // Define mesh matrix
    vector<vector<Point>> points(rows, vector<Point>(columns));

    // Retrieve all matrix points
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            line = in.readLine();
            iter = line.constBegin();
            points[r][c] = parsePoint();
        }
    }

    if (normalAveraging) {
        // Keep track of number of access to point
        vector<vector<int>> faceCount(rows, vector<int>(columns));

        // Find face normals and add to point normals
        for (int r = 0; r < rows-1; r++) {
            for (int c = 0; c < columns-1; c++) {
                // 01   02
                // 03   04

                //Poly1: p1 = 1, p2 = 3, p3 = 4
                //Poly2: p1 = 1, p2 = 4, p3 = 2

                Point v1 = points[r][c];
                Point v2 = points[r][c+1];
                Point v3 = points[r+1][c];
                Point v4 = points[r+1][c+1];

                // Find face normal pf poly 1 and poly 2
                QVector3D normal1 = QVector3D::crossProduct((v3.getWorld() - v1.getWorld()), (v4.getWorld() - v1.getWorld()));
                normal1.normalize();

                QVector3D normal2 = QVector3D::crossProduct((v4.getWorld() - v1.getWorld()), (v2.getWorld() - v1.getWorld()));
                normal2.normalize();

                // Average those normals
                points[r][c].addNormal(normal1);
                points[r][c].addNormal(normal2);
                faceCount[r][c] += 2;

                points[r][c+1].addNormal(normal2);
                faceCount[r][c+1] += 1;

                points[r+1][c].addNormal(normal1);
                faceCount[r+1][c] += 1;

                points[r+1][c+1].addNormal(normal1);
                points[r+1][c+1].addNormal(normal2);
                faceCount[r+1][c+1] += 2;
            }
        }

        // Normalize all vectors
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < columns; c++) {
                points[r][c].normalizeNormal(faceCount[r][c]);
            }
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

            if (!wireframe) {
                QVector3D camera = cam.inverted() * QVector3D(0,0,0);

                if (style == Flat) {
                    Point center = ShaderFlat::shade(pa1, pa2, pa3);
                    QVector<float> intensity = LightingModel::calculateLighting(center, surface, ambient, lights, camera);
                    pa1.setColor(intensity);
                    pa2.setColor(intensity);
                    pa3.setColor(intensity);
                }

                if (style == Gouraud) {
                    ShaderGouraud::shade(pa1, pa2, pa3);
                    pa1.setColor(LightingModel::calculateLighting(pa1, surface, ambient, lights, camera));
                    pa2.setColor(LightingModel::calculateLighting(pa2, surface, ambient, lights, camera));
                    pa3.setColor(LightingModel::calculateLighting(pa3, surface, ambient, lights, camera));
                }

                if (style == Phong) {
                    ShaderPhong::shade(pa1, pa2, pa3);
                }
            }

            Polygon poly1 = Polygon(pa1, pa2, pa3);
            pr.draw_polygon(poly1, drawable, this);

            Point pb1 = points[r][c];
            Point pb2 = points[r+1][c+1];
            Point pb3 = points[r][c+1];

            if (!wireframe) {
                QVector3D camera = cam.inverted() * QVector3D(0,0,0);

                if (style == Flat) {
                    Point center = ShaderFlat::shade(pb1, pb2, pb3);
                    QVector<float> intensity = LightingModel::calculateLighting(center, surface, ambient, lights, camera);
                    pb1.setColor(intensity);
                    pb2.setColor(intensity);
                    pb3.setColor(intensity);
                }

                if (style == Gouraud) {
                    ShaderGouraud::shade(pb1, pb2, pb3);
                    pb1.setColor(LightingModel::calculateLighting(pb1, surface, ambient, lights, camera));
                    pb2.setColor(LightingModel::calculateLighting(pb2, surface, ambient, lights, camera));
                    pb3.setColor(LightingModel::calculateLighting(pb3, surface, ambient, lights, camera));
                }

                if (style == Phong) {
                    ShaderPhong::shade(pb1, pb2, pb3);
                }
            }

            Polygon poly2 = Polygon(pb1, pb2, pb3);
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
