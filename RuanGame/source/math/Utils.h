#pragma once

#include <random>
#include "Vector3.h"

// random float generator with option to change the seed
inline float RandomFloat(float a, float b, uint32_t seed = 0) {
    std::mt19937 gen;

    if (seed != 0) {
        gen.seed(seed);
    }
    else {
        std::random_device rd;
        gen.seed(rd());
    }

    std::uniform_real_distribution<float> dis(a, b);
    return dis(gen);
}

inline float Lerp(const float& a, const float& b, const float& t)
{
    return a * (1.0f - t) + (b * t);
}

inline Vector3 Lerp(const Vector3& a, const Vector3& b, const float& t)
{
    return a + (b - a) * t; // same as unity's lerp
}