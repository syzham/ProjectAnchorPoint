#ifndef ANCHORPOINT_MATH_MATH_H
#define ANCHORPOINT_MATH_MATH_H

#include <cmath>
#include <string>

namespace ap {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 2.0f * kPi;
constexpr float kPiDiv2 = kPi / 2.0f;
constexpr float kPiDiv4 = kPi / 4.0f;

struct Vector2 {
    float x = 0, y = 0;
};

struct Vector3 {
    float x = 0, y = 0, z = 0;

    Vector3 operator+(const Vector3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vector3 operator-(const Vector3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vector3 operator*(const float other) const {
        return {x * other, y * other, z * other};
    }

    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    std::string toString() const {
        return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
    }
};

inline float Dot(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector3 Cross(const Vector3& a, const Vector3& b) {
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}

inline float Length(const Vector3& v) {
    return std::sqrt(Dot(v, v));
}

inline Vector3 Normalize(const Vector3& v) {
    const float len = Length(v);
    if (len == 0.0f) return {0, 0, 0};
    return v * (1.0f / len);
}

// Row-major matrix using the row-vector convention (v' = v * M), matching
// DirectXMath so the existing HLSL shaders keep working unchanged.
struct Matrix4 {
    float m[4][4] = {{1, 0, 0, 0},
                     {0, 1, 0, 0},
                     {0, 0, 1, 0},
                     {0, 0, 0, 1}};

    static Matrix4 Identity() { return {}; }

    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][0] * other.m[0][j]
                               + m[i][1] * other.m[1][j]
                               + m[i][2] * other.m[2][j]
                               + m[i][3] * other.m[3][j];
            }
        }
        return result;
    }

    Matrix4 Transposed() const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[j][i];
        return result;
    }

    static Matrix4 Translation(const Vector3& t) {
        Matrix4 r;
        r.m[3][0] = t.x;
        r.m[3][1] = t.y;
        r.m[3][2] = t.z;
        return r;
    }

    static Matrix4 Scaling(const Vector3& s) {
        Matrix4 r;
        r.m[0][0] = s.x;
        r.m[1][1] = s.y;
        r.m[2][2] = s.z;
        return r;
    }

    static Matrix4 RotationX(float angle) {
        Matrix4 r;
        const float c = std::cos(angle), s = std::sin(angle);
        r.m[1][1] = c;  r.m[1][2] = s;
        r.m[2][1] = -s; r.m[2][2] = c;
        return r;
    }

    static Matrix4 RotationY(float angle) {
        Matrix4 r;
        const float c = std::cos(angle), s = std::sin(angle);
        r.m[0][0] = c; r.m[0][2] = -s;
        r.m[2][0] = s; r.m[2][2] = c;
        return r;
    }

    static Matrix4 RotationZ(float angle) {
        Matrix4 r;
        const float c = std::cos(angle), s = std::sin(angle);
        r.m[0][0] = c;  r.m[0][1] = s;
        r.m[1][0] = -s; r.m[1][1] = c;
        return r;
    }

    static Matrix4 PerspectiveFovLH(float fovY, float aspect, float nearZ, float farZ) {
        Matrix4 r;
        const float h = 1.0f / std::tan(fovY * 0.5f);
        const float w = h / aspect;
        const float range = farZ / (farZ - nearZ);
        r.m[0][0] = w;
        r.m[1][1] = h;
        r.m[2][2] = range;
        r.m[2][3] = 1.0f;
        r.m[3][2] = -range * nearZ;
        r.m[3][3] = 0.0f;
        return r;
    }

    static Matrix4 LookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up) {
        const Vector3 zAxis = Normalize(target - eye);
        const Vector3 xAxis = Normalize(Cross(up, zAxis));
        const Vector3 yAxis = Cross(zAxis, xAxis);

        Matrix4 r;
        r.m[0][0] = xAxis.x; r.m[0][1] = yAxis.x; r.m[0][2] = zAxis.x;
        r.m[1][0] = xAxis.y; r.m[1][1] = yAxis.y; r.m[1][2] = zAxis.y;
        r.m[2][0] = xAxis.z; r.m[2][1] = yAxis.z; r.m[2][2] = zAxis.z;
        r.m[3][0] = -Dot(xAxis, eye);
        r.m[3][1] = -Dot(yAxis, eye);
        r.m[3][2] = -Dot(zAxis, eye);
        return r;
    }
};

} // namespace ap

#endif //ANCHORPOINT_MATH_MATH_H
