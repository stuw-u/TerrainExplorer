#pragma once
#include "../External/glm/vec3.hpp"
#include "../External/glm/common.hpp"

struct VertexData {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;
	float ao;
};