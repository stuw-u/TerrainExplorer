#pragma once
#include "../External/glm/vec3.hpp"

struct AABB {
private:
    glm::vec3 m_min;
    glm::vec3 m_max;
    glm::vec3 m_size;
public:
    AABB (const glm::vec3& size);

    void SetPosition (const glm::vec3& position);

    bool OverlapsWith (const AABB& other) const;
    glm::vec3 GetVNeg (const glm::vec3& normal) const;
    glm::vec3 GetVPos (const glm::vec3& normal) const;

    const glm::vec3& GetSize() const { return m_size; }
};