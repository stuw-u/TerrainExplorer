#pragma once
#include <array>
#include "../External/glm/vec3.hpp"
#include "../External/glm/mat4x4.hpp"
#include "AABB.h"

struct Plane {
    float dist;
    glm::vec3 normal;

    float DistanceToPoint(const glm::vec3& point) const;
};


class Frustum {
private:
    std::array<Plane, 6> m_planes;
public:
    void SetMatrix(glm::mat4x4& projView);
    bool IsPointInFrustum(const glm::vec3& point) const;
    bool IsBoxInFrustum(const AABB& box) const;
};
