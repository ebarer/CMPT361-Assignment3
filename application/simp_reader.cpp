#include "simp_reader.h"

using namespace std;
bool debug = false;

void SimpReader::readFile(QString fileName, Drawable* drawable) {
    ctmStack = QStack<QMatrix4x4>();
    // Define base CTM:
    // [-100, 100] world space, convert to [0, 650] camera space
    ctm = QMatrix4x4();
    ctm.translate(325, 325, 0);
    ctm.scale(3.25, -3.25, 1);

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

        switch((*iter).toLatin1()) {
        case '#':                   // Ignore comments
            continue;
            break;
        case '{':
            ctmStack.push(ctm);
            continue;
            break;
        case '}':
            ctm = ctmStack.pop();
            continue;
            break;
        case 'r':
            parseRotate();
            break;
        case 's':
            parseScale();
            break;
        case 't':
            parseTranslate();
            break;
        case 'l':
            parseLine(drawable);
            break;
        case 'p':
            parsePolygon(drawable);
            break;
        case 'm':
            parseMesh(drawable);
            break;
        case 'w':
            if (debug) { cout << "[WIREFRAME]" << endl; }
            wireframe = true;
            break;
        case 'f':
            if (debug) { cout << "[FILLED]" << endl; }
            wireframe = false;
            break;
        default:
            continue;
            break;
        }
    }

    inputFile.close();
}


// TRANSFORMATIONS
void SimpReader::parseRotate() {
    if (debug) { cout << "ROTATE: "; }
    parseIdentifier();

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
    parseIdentifier();

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
    parseIdentifier();

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
void SimpReader::parseLine(Drawable* drawable) {
    if (debug) { cout << "<LINE>" << endl; }

    parseIdentifier();

    QVector4D v1 = ctm * parsePoint();
    QVector4D v2 = ctm * parsePoint();

    Point p1 = Point(v1, colorNear, colorFar);
    Point p2 = Point(v2, colorNear, colorFar);

    LineRendererDDA br = LineRendererDDA();
    Line line = Line(p1, p2);
    br.draw_line(line, drawable);
}

void SimpReader::parsePolygon(Drawable* drawable) {
    if (debug) { cout << "<POLYGON>" << endl; }

    parseIdentifier();

    QVector4D v1 = ctm * parsePoint();
    QVector4D v2 = ctm * parsePoint();
    QVector4D v3 = ctm * parsePoint();

    Point p1 = Point(v1, colorNear, colorFar);
    Point p2 = Point(v2, colorNear, colorFar);
    Point p3 = Point(v3, colorNear, colorFar);

    PolygonRenderer pr = PolygonRenderer();
    Polygon poly = Polygon(p1, p2, p3);
    pr.draw_polygon(poly, drawable, wireframe);
}

void SimpReader::parseMesh(Drawable* drawable) {
    if (debug) { cout << "<MESH>" << endl; }

    parseIdentifier();

    expect('"');
    QString meshName = parseFilename();
    expect('"');

    if (debug) { cout << meshName.toUtf8().constData() << endl; }
    QFile meshFile(meshName);
    if (!meshFile.open(QIODevice::ReadOnly)) {
        cout << "Couldn't read mesh" << endl;
        return;
    }

    // Preserve iterator position
    QString::const_iterator backupIter = iter;

    QTextStream in(&meshFile);
    QString line = in.readLine();

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
            line = in.readLine();
            iter = line.constBegin();

            QVector4D v = ctm * parseMeshPoint();
            points[r][c] = Point(v, colorNear, colorFar);
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

            pr.draw_polygon(poly1, drawable, wireframe);
            pr.draw_polygon(poly2, drawable, wireframe);
            polyGenerated += 2;
        }
    }

    if (debug) { cout << polyGenerated << endl; }

    // Restore iterator
    iter = backupIter;
}


// PARSERS
void SimpReader::parseIdentifier() {
    readWhitespace();
    while((*iter).isLetter()) {
        iter++;
    }
    readWhitespace();
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

QVector4D SimpReader::parsePoint() {
    float x, y, z;
    QVector4D v;

    expect('(');
    x = parseDigit();
    readSeparator();
    y = parseDigit();
    readSeparator();
    z = parseDigit();
    expect(')');
    readWhitespace();

    v = QVector4D(x, y, z, 1);
    return v;
}

QVector4D SimpReader::parseMeshPoint() {
    float x, y, z;
    QVector4D v;

    x = parseDigit();
    readWhitespace();
    y = parseDigit();
    readWhitespace();
    z = parseDigit();
    readWhitespace();

    v = QVector4D(x, y, z, 1);
    return v;
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
