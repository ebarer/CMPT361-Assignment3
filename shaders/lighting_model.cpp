#include "lighting_model.h"

QVector<float> LightingModel::calculateLighting(Point p,
                                                Surface surface,
                                                Ambient ambient,
                                                QVector<Light> lights,
                                                QVector3D camera) {

    QVector<float> intensity = QVector<float>(3);

// Evaluate RED intensity
    for (int c = 0; c < 3; c++) {
        float kd = p.getChannels()[c];
        float Ia = ambient.colors[c];
        float sum = 0;

        for (int i = 0; i < lights.size(); i++) {
            float di = lights[i].p.getLoc().distanceToPoint(p.getWorld());
            float fatti = 1 / (lights[i].attA + (lights[i].attB * di));

            QVector3D L = lights[i].p.getLoc() - p.getWorld();     // world vs. camera? subtract not cross?
            float NL = QVector3D::dotProduct(p.getNormal(), L);
            float kdNL =  kd * NL;

            QVector3D R = (2 * NL * p.getNormal()) - L;
            QVector3D V = camera - p.getLoc();
            V.normalize();
            float ksVR = surface.ks * pow(QVector3D::dotProduct(V, R), surface.alpha);

            sum += lights[i].colors[c] * fatti * (kdNL + ksVR);
        }

        intensity[c] = kd * Ia + sum;
    }

// Return muxed channels
    return intensity;
}
