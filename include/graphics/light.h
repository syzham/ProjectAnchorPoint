#ifndef PROJECTANCHORPOINT_LIGHT_H
#define PROJECTANCHORPOINT_LIGHT_H

#include <vector>
#include "graphics/graphics.h"
#include "common.h"

enum LightType {
    Directional,
    Point,
    Spot
};

struct Light {
    LightType type;
    Vector3 position;
    float range;
    Vector3 direction;
    Vector3 color;
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
