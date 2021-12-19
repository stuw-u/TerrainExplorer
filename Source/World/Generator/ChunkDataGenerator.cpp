#include "ChunkDataGenerator.h"
#include "MeshGenerator.h"
#include "../../External/FastNoiseLite/FastNoiseLite.h"
#include "Utils/TerrainUtils.h"
#include <vector>

const glm::vec3 ChunkDataGenerator::samples[] = {
	glm::vec3(0,0,0),
	glm::vec3(1,0,0), glm::vec3(-1,0,0),
	glm::vec3(0,1,0), glm::vec3(0,-1,0),
	glm::vec3(0,0,1), glm::vec3(0,0,-1)
};

ChunkDataGenerator::ChunkDataGenerator(
	int seed, uint8_t size, uint8_t gridSize, BlockAssetManager* blockAssetManager) :
	m_size(size), m_gridSize(gridSize), m_seed(seed), m_blockAssetManager(blockAssetManager) {}

void ChunkDataGenerator::GridDataPass(Chunk* chunk) {

	// Noise samplers
	FastNoiseLite baseNoise;
	baseNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	baseNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
	baseNoise.SetFractalOctaves(7);
	baseNoise.SetFractalGain(0.65f);
	baseNoise.SetFrequency(0.002f);
	FastNoiseLite cellNoise;
	cellNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	cellNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
	cellNoise.SetFrequency(0.02f);
	std::vector<FastNoiseLite> offsets(8);
	for(int i = 0; i < 8; i++) {
		FastNoiseLite noiseOffset;
		noiseOffset.SetSeed(i + 1);
		noiseOffset.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noiseOffset.SetFractalType(FastNoiseLite::FractalType_FBm);
		noiseOffset.SetFractalOctaves(1);
		offsets[i] = noiseOffset;
	}
	for(int i = 0; i < 3; i++) {
		offsets[i * 2].SetFrequency(0.002f);
		offsets[i * 2 + 1].SetFrequency(0.006f);
	}
	for(int i = 6; i < 8; i++) {
		offsets[i].SetFractalOctaves(3);
		offsets[i].SetFrequency(0.005f);
	}

	std::srand(m_seed);
	glm::vec3 chunkPos = chunk->chunkPosition;
	int chunkSize = chunk->data.size();
	int blockPerGrid = chunkSize / m_gridSize;

	std::vector<float> noiseSamples(7);

	for(uint8_t i = 0; i < m_gridSize; i++) {
		for(uint8_t j = 0; j < m_gridSize; j++) {
			for(uint8_t k = 0; k < m_gridSize; k++) {

				double centerX = i * blockPerGrid + chunkPos.x * chunkSize,
					   centerY = j * blockPerGrid + chunkPos.y * chunkSize,
					   centerZ = k * blockPerGrid + chunkPos.z * chunkSize;

				for(int sample = 0; sample < 7; sample++) {

					double x = centerX + samples[sample].x * m_gridSize;
					double y = centerY + samples[sample].y * m_gridSize;
					double z = centerZ + samples[sample].z * m_gridSize;

					double beachFade = TUtils::invLerpClamp(y, 0, 30);

					double offsetX = offsets[6].GetNoise(x, y, z) * 0.4,
						offsetZ = offsets[7].GetNoise(x, y, z) * 0.4;

					float value = (
						(baseNoise.GetNoise(x + offsetX, z + offsetZ) - 0.8) +
						(1.0 - cellNoise.GetNoise(x + offsetX * 0.5, z + offsetZ * 0.5)) * 1.1) * 50;


					for(int step = 0; step < 3; step++) {
						value = TUtils::lerp(beachFade,
							value,
							TUtils::terrace(
								value,
								TUtils::lerp(offsets[step * 2].GetNoise(x, y, z), 30, 50) * (1.0 - step * 0.2),
								TUtils::lerp(offsets[step * 2 + 1].GetNoise(x, y, z) + step * 0.33, 0, 1),
								0.0
							));
					}

				}

				glm::lowp_vec3 normal = glm::normalize(glm::lowp_vec3(
					(noiseSamples[2] - noiseSamples[1]) / m_gridSize,
					(noiseSamples[4] - noiseSamples[3]) / m_gridSize,
					(noiseSamples[6] - noiseSamples[5]) / m_gridSize
				));
				chunk->gridData.SetCell(i, j, k, noiseSamples[0], normal);
			}
		}
	}
}

void ChunkDataGenerator::ChunkDataPass(Chunk* chunk, std::vector<Chunk*>& chunks) {
	BlockAsset* empty = m_blockAssetManager->GetBlockAssetFromCodeName("empty");
	BlockAsset* stone = m_blockAssetManager->GetBlockAssetFromCodeName("stone");

	std::srand(m_seed);
	glm::vec3 chunkPos = chunk->chunkPosition;
	int chunkSize = chunk->data.size();
	int blockPerGrid = chunkSize / m_gridSize;

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
					chunk->data.SetCell(i, j, k, empty);
				}
				else {
					chunk->data.SetCell(i, j, k, stone);
				}
			}
		}
	}
}

void ChunkDataGenerator::SurfacePass (Chunk* chunk, std::vector<Chunk*>& chunks) {
	BlockAsset* stone = m_blockAssetManager->GetBlockAssetFromCodeName("stone");
	BlockAsset* grass = m_blockAssetManager->GetBlockAssetFromCodeName("grass");
	BlockAsset* dirt = m_blockAssetManager->GetBlockAssetFromCodeName("dirt");

	glm::vec3 chunkPos = chunk->chunkPosition;
	int chunkSize = chunk->data.size();
	int blockPerGrid = chunkSize / m_gridSize;

	for(uint8_t i = 0; i < m_size; i++) {
		for(uint8_t j = 0; j < m_size; j++) {
			for(uint8_t k = 0; k < m_size; k++) {
				BlockAsset* centerAsset = chunk->data.GetCell(i, j, k);
				if(centerAsset->id == 0)
					continue;

				float x = i + chunkPos.x * chunkSize,
					  y = j + chunkPos.y * chunkSize,
					  z = k + chunkPos.z * chunkSize;

				/*glm::vec3 normal = SampleGridNormal(
					i / static_cast<double>(blockPerGrid),
					j / static_cast<double>(blockPerGrid),
					k / static_cast<double>(blockPerGrid),
					chunks);


				float dotUp = glm::dot(glm::vec3(0, 1, 0), normal);
				if(dotUp < 0.5f) {
					chunk->data.SetCell(i, j, k, stone);
					continue;
				}*/

				int index = 0;
				for(int x = 1; x < 6; x++) {
					BlockAsset* bA = MeshGenerator::GetCellInChunks(glm::ivec3(i, j + x, k), chunks);
					if(bA == nullptr || bA->id == 0)
						break;
					index++;
				}
				if(index == 0)
					chunk->data.SetCell(i, j, k, grass);
				else if(index >= 5)
					chunk->data.SetCell(i, j, k, stone);
				else
					chunk->data.SetCell(i, j, k, dirt);
			}
		}
	}
}

float ChunkDataGenerator::GetCellValue(int x, int y, int z, std::vector<Chunk*>& chunks) {
	int cellX = x % m_gridSize;
	int cellY = y % m_gridSize;
	int cellZ = z % m_gridSize;
	int gridX = x / m_gridSize;
	int gridY = y / m_gridSize;
	int gridZ = z / m_gridSize;
	int chunkIndex = gridX + gridY * 2 + gridZ * 4;
	return (*chunks[chunkIndex]).gridData.GetValue(cellX, cellY, cellZ);
}

glm::lowp_vec3 ChunkDataGenerator::GetDerivativeValue(int x, int y, int z, std::vector<Chunk*>& chunks) {
	int cellX = x % m_gridSize;
	int cellY = y % m_gridSize;
	int cellZ = z % m_gridSize;
	int gridX = x / m_gridSize;
	int gridY = y / m_gridSize;
	int gridZ = z / m_gridSize;
	int chunkIndex = (gridX + 1) + (gridY + 1) * 3 + (gridZ + 1) * 9;
	return (*chunks[chunkIndex]).gridData.GetDerivatives(cellX, cellY, cellZ);
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

	return TUtils::vec3lerp(fz,
		TUtils::vec3Bilinear(fx, fy, cell000, cell010, cell100, cell110),
		TUtils::vec3Bilinear(fx, fy, cell001, cell011, cell101, cell111));
}