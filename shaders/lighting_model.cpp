#include "lighting_model.h"

QVector<float> LightingModel::calculateLighting(Point p,
                                                Surface surface,
                                                Ambient ambient,
                                                QVector<Light> lights,
                                                QVector3D camera) {

    QVector<float> intensity = QVector<float>(3);

    // Set ambient lighting
    for (int c = 0; c < 3; c++) {
        float Ia = ambient.colors[c];
        float kd = p.getChannels()[c] / 255.0;
        intensity[c] = kd * Ia;
    }

    // Set lighting based on lights in scene
    for (int i = 0; i < lights.size(); i++) {
        float di = lights[i].p.getWorld().distanceToPoint(p.getWorld());
        float fatti = 1 / (lights[i].attA + (lights[i].attB * di));

        QVector3D L = lights[i].p.getWorld() - p.getWorld();     // world vs. camera? subtract not cross?
        L.normalize();
        QVector3D N = p.getNormal();
        N.normalize();
        float NL = QVector3D::dotProduct(N, L);

        QVector3D R = (2 * NL * N) - L;
        R.normalize();
        QVector3D V = camera - p.getWorld();
        V.normalize();
        float ksVR = surface.ks * pow(QVector3D::dotProduct(V, R), surface.alpha);

        for (int c = 0; c < 3; c++) {
            float kd = p.getChannels()[c] / 255.0;
            float kdNL = kd * NL;
            float sum = lights[i].colors[c] * fatti * (kdNL + ksVR);
            intensity[c] += sum;

            // If VAL is < 0 or > 1, set to respective limit, else set to VAL
            intensity[c] = (intensity[c] > 1) ? 1 : (intensity[c] < 0) ? 0 : intensity[c];
        }
    }

    return intensity;
}
