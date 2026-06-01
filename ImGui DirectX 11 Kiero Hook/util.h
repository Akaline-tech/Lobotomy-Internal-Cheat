#include "data.h"
#pragma once
inline float Distance(const Vector3& a, const Vector3& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

inline float Distance2D(const Vector3& a, const Vector3& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

#define cnull(a,b,c) if (!a)\
{\
std::cout << "[!] "<< b << " " << c << " not found" << std::endl;\
return;\
}
