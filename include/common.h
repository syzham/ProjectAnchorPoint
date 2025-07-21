#ifndef PROJECTANCHORPOINT_COMMON_H
#define PROJECTANCHORPOINT_COMMON_H

#include <string>

struct Vector3 {
    float x, y, z;

    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3& operator+=(const Vector3& other) {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }

    std::string toString() const {
        return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
    }
};

#endif //PROJECTANCHORPOINT_COMMON_H
