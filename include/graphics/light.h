#ifndef PROJECTANCHORPOINT_LIGHT_H
#define PROJECTANCHORPOINT_LIGHT_H

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

#endif //PROJECTANCHORPOINT_LIGHT_H
