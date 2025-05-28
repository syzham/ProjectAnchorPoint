#ifndef PROJECTANCHORPOINT_LIGHT_H
#define PROJECTANCHORPOINT_LIGHT_H

#include <vector>
#include "graphics/graphics.h"

enum LightType {
    Directional,
    Point,
    Spot
};

struct Light {
    LightType type;
    float position[3];
    float range;
    float direction[3];
    float color[3];
    float intensity;
};

class Lights {
public:
    static std::vector<Light> activeLights;
    static void Init();
    static void Update();
    static void Release();

private:
    static ID3D11Buffer* lightBuffer;
    static ID3D11ShaderResourceView* lightSRV;
};

#endif //PROJECTANCHORPOINT_LIGHT_H
