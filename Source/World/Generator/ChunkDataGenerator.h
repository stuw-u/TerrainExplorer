#pragma once
#include "../Chunk/Chunk.h"
#include <random>

class ChunkDataGenerator {
private:
	uint8_t m_size;
	uint8_t m_gridSize;
	int m_seed;
	BlockAssetManager* m_blockAssetManager;

	static const glm::vec3 enumToNormal[6];


	float GetCellValue(int x, int y, int z, std::vector<Chunk*>& chunks);

	glm::lowp_vec3 GetDerivativeValue(int x, int y, int z, std::vector<Chunk*>& chunks);
	
	float SampleGridValue (float x, float y, float z, std::vector<Chunk*>& chunks);

	glm::lowp_vec3 SampleGridNormal (float x, float y, float z, std::vector<Chunk*>& chunks);

	static const glm::vec3 samples[7];

public:
	ChunkDataGenerator(
		int seed, uint8_t size, uint8_t gridSize,
		BlockAssetManager* blockAssetManager);

	ChunkDataGenerator(): m_size(0), m_gridSize(0), m_seed(0), m_blockAssetManager(nullptr) {}

	void GridDataPass (Chunk* chunk);

	void ChunkDataPass (Chunk* chunk, std::vector<Chunk*>& chunks);

	void SurfacePass (Chunk* chunk, std::vector<Chunk*>& chunks);
};