#pragma once
#include "../../External/glm/vec3.hpp"
#include "../../External/glm/glm.hpp"
#include "../Generator/Utils/TerrainUtils.h"
#include <vector>
#include <cstdint>
#include <cmath>

class ChunkGridData {
	uint8_t m_size;
	std::vector<float> m_cells;
	std::vector<glm::lowp_vec3> m_derivatives;

public:
	ChunkGridData(uint8_t size) : m_size(size), m_cells(m_size * m_size * m_size),
	m_derivatives(m_size * m_size * m_size) {}

	float GetValue (uint8_t x, uint8_t y, uint8_t z) {
		long long index = x + y * static_cast<uint64_t>(m_size) + z * static_cast<uint64_t>(m_size) * m_size;
		return m_cells[index];
	}

	glm::lowp_vec3 GetDerivatives (uint8_t x, uint8_t y, uint8_t z) {
		long long index = x + y * static_cast<uint64_t>(m_size) + z * static_cast<uint64_t>(m_size) * m_size;
		return m_derivatives[index];
	}

	void SetCell(uint8_t x, uint8_t y, uint8_t z, float value) {
		long long index = x + y * static_cast<uint64_t>(m_size) + z * static_cast<uint64_t>(m_size) * m_size;
		m_cells[index] = value;
	}

	void SetDerivative(uint8_t x, uint8_t y, uint8_t z, glm::lowp_vec3 derivative) {
		long long index = x + y * static_cast<uint64_t>(m_size) + z * static_cast<uint64_t>(m_size) * m_size;
		m_derivatives[index] = derivative;
	}
};