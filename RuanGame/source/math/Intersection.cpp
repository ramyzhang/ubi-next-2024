#include "stdafx.h"

#include "Intersection.h"

// returns earliest intersection distance in the t variable - this is the analytic solution
bool IntersectRaySphere(const Vector3& ray_origin, Vector3 ray_dir, float& depth, const CCollider& sphere) {
    Vector3 ray_norm = ray_dir.normalize();

    Vector3 dist = sphere.center - ray_origin;
    float closest = dist.dot(ray_norm);

    if (closest < 0) return false; // if dotprod is negative, then the ray points away from the sphere

    float dist2 = dist.dot(dist) - closest * closest; // squared distance
    if (dist2 > sphere.radius * sphere.radius) return false; // ray missed the sphere

    float half_chord = sqrtf(sphere.radius * sphere.radius - dist2);
    float t0 = closest - half_chord;
    float t1 = closest + half_chord;

    float length = ray_dir.magnitude();
    depth = (t0 < 0) ? t1 : t0; // return the closest non-negative intersection distance
    return (depth >= 0 && length >= depth);
}

// based on the ScratchAPixel math
bool IntersectRayAABB(const Vector3& ray_origin, const Vector3& ray_dir, float& depth, const CCollider& aabb) {
    Vector3 min_aabb = aabb.center - aabb.half_size;
    Vector3 max_aabb = aabb.center + aabb.half_size;

    float min_t;
    float max_t;

    // let's try to avoid divide by 0 error
    // first, calculate intersections with the x plane (facing left and right)
    float t0x = (min_aabb.x - ray_origin.x) / (ray_dir.x != 0 ? ray_dir.x : 0.00001f);
    float t1x = (max_aabb.x - ray_origin.x) / (ray_dir.x != 0 ? ray_dir.x : 0.00001f);

    min_t = std::min(t0x, t1x); // if t1x or t1y is bigger, then our math doesn't work lol
    max_t = std::max(t0x, t1x);

    // then, calculate intersections with the y plane (facing bottom and top)
    float t0y = (min_aabb.y - ray_origin.y) / (ray_dir.y != 0 ? ray_dir.y : 0.00001f);
    float t1y = (max_aabb.y - ray_origin.y) / (ray_dir.y != 0 ? ray_dir.y : 0.00001f);

    // if 
    min_t = std::max(min_t, std::min(t0y, t1y));
    max_t = std::min(max_t, std::max(t0y, t1y));

    // then, calculate intersections with the z plan (facing front and back)
    float t0z = (min_aabb.z - ray_origin.z) / (ray_dir.z != 0 ? ray_dir.z : 0.00001f);
    float t1z = (max_aabb.z - ray_origin.z) / (ray_dir.z != 0 ? ray_dir.z : 0.00001f);

    min_t = std::max(min_t, std::min(t0z, t1z));
    max_t = std::min(max_t, std::max(t0z, t1z));

    depth = min_t;
    float length = ray_dir.magnitude();
    return (max_t >= min_t && max_t >= 0 && min_t <= length);
}