#pragma once
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <math.h>

struct Vector3 {

    float x;
    float y;
    float z;
    float w = 1;
    
    Vector3() : x(0), y(0), z(0), w(1) {};
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
    Vector3(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {};

    Vector3 operator + (const Vector3& rhs) const;
    Vector3 operator - (const Vector3& rhs) const;
    Vector3 operator * (const float& rhs) const;

    Vector3& add(const Vector3& rhs);
    Vector3& subtract(const Vector3& rhs);
    Vector3& scale(const float& rhs);
    Vector3& divideByZ();
    Vector3& divideByW();

    float distance(const Vector3& v) const;
    Vector3& normalize();
    float dot(const Vector3& v) const;
    Vector3 cross(const Vector3& rhs) const;

};

inline Vector3 Vector3::operator + (const Vector3& rhs) const {
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

inline Vector3 Vector3::operator - (const Vector3& rhs) const {
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

inline Vector3 Vector3::operator * (const float& rhs) const {
    return Vector3(x * rhs, y * rhs, z * rhs);
}

inline Vector3& Vector3::add(const Vector3& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

inline Vector3& Vector3::subtract(const Vector3& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

inline Vector3& Vector3::scale(const float& rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

inline Vector3& Vector3::divideByZ() {
    if (z == 0.0f) return *this;

    x /= z;
    y /= z;
    z /= z;

    return *this;
}

inline Vector3& Vector3::divideByW() {
    if (w == 0.0f) return *this;

    x /= w;
    y /= w;
    z /= w;
    w = 1;

    return *this;
}

inline float Vector3::distance(const Vector3& v) const {
    return sqrtf((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z));
}

inline Vector3& Vector3::normalize() {
    float norm = sqrtf(x * x + y * y + z * z);

    if (norm == 0.0) return *this;

    x = x / norm;
    y = y / norm;
    z = z / norm;

    return *this;
}

inline float Vector3::dot(const Vector3& v) const {
    return (x * v.x + y * v.y + z * v.z);
}

inline Vector3 Vector3::cross(const Vector3& v) const {
    return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}