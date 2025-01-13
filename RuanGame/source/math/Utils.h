#pragma once
#include <stdlib.h>

float RandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

inline float Lerp(float a, float b, float t)
{
    return a * (1.0 - t) + (b * t);
}

inline Vector3 Lerp(Vector3 a, Vector3 b, float t)
{
    return a + (b - a) * t;
}