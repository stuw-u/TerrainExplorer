#pragma once
#include "../Chunk/Chunk.h"
#include <random>

class ChunkDataGenerator {
private:
	uint8_t m_size;
	int m_seed;
	BlockAssetManager* m_blockAssetManager;
public:
	ChunkDataGenerator(int seed, uint8_t size, BlockAssetManager* blockAssetManager) {
		m_size = size;
		m_seed = seed;
		m_blockAssetManager = blockAssetManager;
	}

	ChunkDataGenerator(): m_size(0), m_seed(0), m_blockAssetManager(nullptr) {}

	void GenerateChunkData(Chunk* chunk) {

		BlockAsset* empty = m_blockAssetManager->GetBlockAssetFromCodeName("empty");
		BlockAsset* stone = m_blockAssetManager->GetBlockAssetFromCodeName("stone");

		std::srand(m_seed); // TODO: Shuffle seed based on chunk position or sum
		for(uint8_t x = 0; x < m_size; x++) {
			for(uint8_t y = 0; y < m_size; y++) {
				for(uint8_t z = 0; z < m_size; z++) {

					float randomValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
					if(randomValue < (static_cast<float>(y) / m_size)) {
						chunk->data.SetCell(x, y, z, empty);
					}
					else {
						chunk->data.SetCell(x, y, z, stone);
					}
				}
			}
		}
	}
};