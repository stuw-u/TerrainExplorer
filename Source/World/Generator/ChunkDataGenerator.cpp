#include "ChunkDataGenerator.h"
#include "MeshGenerator.h"
#include "../../External/FastNoiseLite/FastNoiseLite.h"
#include "Utils/TerrainUtils.h"
#include "NoiseFunctions/TestNoise.h"
#include <vector>



ChunkDataGenerator::ChunkDataGenerator(
	int seed, uint8_t size, uint8_t gridSize, BlockAssetManager* blockAssetManager) :
	m_size(size), m_gridSize(gridSize), m_seed(seed), m_blockAssetManager(blockAssetManager) {}

void ChunkDataGenerator::GridDataPass(Chunk* chunk) {

	std::srand(m_seed);
	glm::vec3 chunkPos = chunk->chunkPosition;
	int chunkSize = chunk->data.size();
	int blockPerGrid = chunkSize / m_gridSize;

	// Noise samplers
	YosemiteValley_Context sampler0;

	for(uint8_t i = 0; i < m_gridSize; i++) {
		for(uint8_t j = 0; j < m_gridSize; j++) {
			for(uint8_t k = 0; k < m_gridSize; k++) {

				double x = i * blockPerGrid + chunkPos.x * chunkSize,
					  y = j * blockPerGrid + chunkPos.y * chunkSize,
					  z = k * blockPerGrid + chunkPos.z * chunkSize;

				

				chunk->gridData.SetCell(i, j, k, sampler0.Sample(x, y, z));

			}
		}
	}
}

void ChunkDataGenerator::ChunkDataPass(Chunk* chunk, std::vector<Chunk*>& chunks) {
	uint16_t empty = chunk->data.GetPaletteIndex(m_blockAssetManager->GetBlockAssetFromCodeName("empty"));
	uint16_t stone = chunk->data.GetPaletteIndex(m_blockAssetManager->GetBlockAssetFromCodeName("stone"));

	std::srand(m_seed);
	glm::vec3 chunkPos = chunk->chunkPosition;
	int chunkSize = chunk->data.size();
	int blockPerGrid = chunkSize / m_gridSize;

	int filledCorners = 0;
	for(uint8_t i = 0; i <= m_gridSize; i++) {
		for(uint8_t j = 0; j <= m_gridSize; j++) {
			for(uint8_t k = 0; k <= m_gridSize; k++) {
				double y = j * blockPerGrid + chunkPos.y * chunkSize;
				filledCorners += (GetCellValue(i, j, k, chunks) < y) ? 0 : 1;
			}
		}
	}

	if(filledCorners == 0) {
		chunk->data.FillCellsUnsafe(empty);
		return;
	}
	else if(filledCorners == ((m_gridSize + 1) * (m_gridSize + 1) * (m_gridSize + 1))) {
		chunk->data.FillCellsUnsafe(stone);
		return;
	}

	for(uint8_t i = 0; i < m_size; i++) {
		for(uint8_t j = 0; j < m_size; j++) {
			for(uint8_t k = 0; k < m_size; k++) {

				double x = i + chunkPos.x * chunkSize,
					   y = j + chunkPos.y * chunkSize,
					   z = k + chunkPos.z * chunkSize;

				float value = SampleGridValue(
					i / static_cast<double>(blockPerGrid),
					j / static_cast<double>(blockPerGrid),
					k / static_cast<double>(blockPerGrid),
					chunks);

				if(value < y) {
					chunk->data.SetCellUnsafe(i, j, k, empty);
				}
				else {
					chunk->data.SetCellUnsafe(i, j, k, stone);
				}
			}
		}
	}

	for(uint8_t k = 0; k < m_gridSize; k++) {
		for(uint8_t j = 0; j < m_gridSize; j++) {
			for(uint8_t i = 0; i < m_gridSize; i++) {

				glm::lowp_vec3 normal = glm::normalize(glm::vec3(
					(GetCellValue(i - 1, j, k, chunks) - GetCellValue(i + 1, j, k, chunks)),
					(GetCellValue(i, j - 1, k, chunks) - GetCellValue(i, j + 1, k, chunks)) + 1.0f,
					(GetCellValue(i, j, k - 1, chunks) - GetCellValue(i, j, k + 1, chunks))
				));
				chunk->gridData.SetDerivative(i, j, k, normal);
			}
		}
	}
}

void ChunkDataGenerator::SurfacePass (Chunk* chunk, std::vector<Chunk*>& chunks) {
	uint16_t stone = chunk->data.GetPaletteIndex(m_blockAssetManager->GetBlockAssetFromCodeName("stone"));
	uint16_t grass = chunk->data.GetPaletteIndex(m_blockAssetManager->GetBlockAssetFromCodeName("grass"));
	uint16_t dirt = chunk->data.GetPaletteIndex(m_blockAssetManager->GetBlockAssetFromCodeName("dirt"));

	std::srand(m_seed);
	glm::vec3 chunkPos = chunk->chunkPosition;
	int chunkSize = chunk->data.size();
	int blockPerGrid = chunkSize / m_gridSize;

	for(int i = 0; i < m_size; i++) {
		for(int j = m_size - 1; j >= 0; j--) {
			for(int k = 0; k < m_size; k++) {
				BlockAsset* centerAsset = chunk->data.GetCell(i, j, k);
				if(centerAsset->id == 0)
					continue;

				// Solve case where we absolutely know the solution;
				if(j < m_size - 1) {
					uint16_t topId = chunk->data.GetCellPaletteIndex(i, j + 1, k);
					if(topId == stone) {
						continue;
					}
					else if(topId == dirt) {
						chunk->data.SetCellUnsafe(i, j, k, dirt);
						continue;
					}
				}

				float x = i + chunkPos.x * chunkSize,
					  y = j + chunkPos.y * chunkSize,
					  z = k + chunkPos.z * chunkSize;

				glm::vec3 normal = SampleGridNormal(
					i / static_cast<double>(blockPerGrid),
					j / static_cast<double>(blockPerGrid),
					k / static_cast<double>(blockPerGrid),
					chunks);


				float dotUp = glm::dot(glm::vec3(0, 1, 0), normal);
				if(dotUp < 0.2f) {
					chunk->data.SetCellUnsafe(i, j, k, stone);
					continue;
				}

				int height = rand() % (7 - 5 + 1) + 5; // 7 is max, 5 is min
				int index = 0;
				for(int x = 1; x < height; x++) {
					BlockAsset* bA = MeshGenerator::GetCellInChunks(glm::ivec3(i, j + x, k), chunks);
					if(bA == nullptr || bA->id == 0)
						break;
					index++;
				}
				if(index == 0)
					chunk->data.SetCellUnsafe(i, j, k, grass);
				else if(index >= height - 1)
					chunk->data.SetCellUnsafe(i, j, k, stone);
				else
					chunk->data.SetCellUnsafe(i, j, k, dirt);
			}
		}
	}
}

float ChunkDataGenerator::GetCellValue(int x, int y, int z, std::vector<Chunk*>& chunks) {
	int gridX = std::floorl(x / (double)m_gridSize);
	int gridY = std::floorl(y / (double)m_gridSize);
	int gridZ = std::floorl(z / (double)m_gridSize);
	int cellX = x - gridX * m_gridSize;
	int cellY = y - gridY * m_gridSize;
	int cellZ = z - gridZ * m_gridSize;
	int chunkIndex = (gridX + 1) + (gridY + 1) * 3 + (gridZ + 1) * 9;
	return chunks[chunkIndex]->gridData.GetValue(cellX, cellY, cellZ);
}

glm::lowp_vec3 ChunkDataGenerator::GetDerivativeValue(int x, int y, int z, std::vector<Chunk*>& chunks) {
	int gridX = std::floorl(x / (double)m_gridSize);
	int gridY = std::floorl(y / (double)m_gridSize);
	int gridZ = std::floorl(z / (double)m_gridSize);
	int cellX = x - gridX * m_gridSize;
	int cellY = y - gridY * m_gridSize;
	int cellZ = z - gridZ * m_gridSize;
	int chunkIndex = (gridX + 1) + (gridY + 1) * 3 + (gridZ + 1) * 9;
	return chunks[chunkIndex]->gridData.GetDerivatives(cellX, cellY, cellZ);
}

float ChunkDataGenerator::SampleGridValue (float x, float y, float z, std::vector<Chunk*>& chunks) {
	float fx = glm::fract(x);
	float fy = glm::fract(y);
	float fz = glm::fract(z);
	int cellX = std::floorl(x);
	int cellY = std::floorl(y);
	int cellZ = std::floorl(z);
	float cell000 = GetCellValue(cellX + 0, cellY + 0, cellZ + 0, chunks);
	float cell100 = GetCellValue(cellX + 1, cellY + 0, cellZ + 0, chunks);
	float cell010 = GetCellValue(cellX + 0, cellY + 1, cellZ + 0, chunks);
	float cell110 = GetCellValue(cellX + 1, cellY + 1, cellZ + 0, chunks);
	float cell001 = GetCellValue(cellX + 0, cellY + 0, cellZ + 1, chunks);
	float cell101 = GetCellValue(cellX + 1, cellY + 0, cellZ + 1, chunks);
	float cell011 = GetCellValue(cellX + 0, cellY + 1, cellZ + 1, chunks);
	float cell111 = GetCellValue(cellX + 1, cellY + 1, cellZ + 1, chunks);

	return TUtils::flerp(fz,
		TUtils::fBilinear(fx, fy, cell000, cell010, cell100, cell110),
		TUtils::fBilinear(fx, fy, cell001, cell011, cell101, cell111));
}

glm::lowp_vec3 ChunkDataGenerator::SampleGridNormal (float x, float y, float z, std::vector<Chunk*>& chunks) {
	float fx = glm::fract(x);
	float fy = glm::fract(y);
	float fz = glm::fract(z);
	int cellX = std::floorl(x);
	int cellY = std::floorl(y);
	int cellZ = std::floorl(z);
	glm::lowp_vec3 cell000 = GetDerivativeValue(cellX + 0, cellY + 0, cellZ + 0, chunks);
	glm::lowp_vec3 cell100 = GetDerivativeValue(cellX + 1, cellY + 0, cellZ + 0, chunks);
	glm::lowp_vec3 cell010 = GetDerivativeValue(cellX + 0, cellY + 1, cellZ + 0, chunks);
	glm::lowp_vec3 cell110 = GetDerivativeValue(cellX + 1, cellY + 1, cellZ + 0, chunks);
	glm::lowp_vec3 cell001 = GetDerivativeValue(cellX + 0, cellY + 0, cellZ + 1, chunks);
	glm::lowp_vec3 cell101 = GetDerivativeValue(cellX + 1, cellY + 0, cellZ + 1, chunks);
	glm::lowp_vec3 cell011 = GetDerivativeValue(cellX + 0, cellY + 1, cellZ + 1, chunks);
	glm::lowp_vec3 cell111 = GetDerivativeValue(cellX + 1, cellY + 1, cellZ + 1, chunks);

	return glm::normalize(TUtils::vec3lerp(fz,
		TUtils::vec3Bilinear(fx, fy, cell000, cell010, cell100, cell110),
		TUtils::vec3Bilinear(fx, fy, cell001, cell011, cell101, cell111)));
}