#include "AABB.h"


AABB::AABB(const glm::vec3& size) : m_size(size), m_min(), m_max(size) { }

void AABB::SetPosition (const glm::vec3& position) {
    m_min = position;
    m_max = m_min + m_size;
}

bool AABB::OverlapsWith (const AABB& other) const {
    return  (m_min.x <= other.m_max.x && m_max.x >= m_min.x) &&
        (m_min.y <= other.m_max.y && m_max.y >= m_min.y) &&
        (m_min.z <= other.m_max.z && m_max.z >= m_min.z);
}

glm::vec3 AABB::GetVNeg (const glm::vec3& normal) const {
    glm::vec3 res = m_min;

    if(normal.x < 0) {
        res.x += m_size.x;
    }
    if(normal.y < 0) {
        res.y += m_size.y;
    }
    if(normal.z < 0) {
        res.z += m_size.z;
    }

    return res;
}

glm::vec3 AABB::GetVPos (const glm::vec3& normal) const  {
    glm::vec3 res = m_min;

    if(normal.x > 0) {
        res.x += m_size.x;
    }
    if(normal.y > 0) {
        res.y += m_size.y;
    }
    if(normal.z > 0) {
        res.z += m_size.z;
    }

    return res;
}