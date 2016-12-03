#ifndef LIGHTING_MODEL_H
#define LIGHTING_MODEL_H

#include "primitives/gfx_basics.h"
#include "shaders/shade_basics.h"
#include "math.h"

class LightingModel
{
public:
    LightingModel() {}

    static QVector<float> calculateLighting(Point p,
                                            Surface surface,
                                            Ambient ambient,
                                            QVector<Light> lights,
                                            QVector3D camera);

};

#endif // LIGHTING_MODEL_H
