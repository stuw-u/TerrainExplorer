#pragma once
#include <array>
#include <vector>
#include <map>
#include <cstdint>
#include "../Asset/BlockAsset.h"
#include "../Asset/BlockAssetManager.h"

// This class is not safe. Don't access cells outside the N range.
// TODO: Make it so bounds are checked only in debug mode?
// Is the way I'm trying to do it too slow?

// We have the run-time determined id of the block.
// The generator wants to set a certain block to a certain block.
// We want to figure out the palette id from a block id as fast as possible.
// Let's use a dict. that maps block id (key) with palette id (value)

// N is the size of one side of a chunk
class ChunkData{
private:
	uint8_t m_size;
	BlockAssetManager* m_context;
	std::vector<uint32_t> m_palette;									// A palette of block asset used in the chunk
	std::map<uint32_t, uint16_t> m_blockIdToPaletteIndex;				// Maps runtime block id to palette index
	std::vector<uint16_t> m_cells;										// An index for a palette (See: 1.13 minecraft flattening)

public:
	ChunkData() = delete;
	ChunkData(uint8_t size, BlockAssetManager* context) {
		m_context = context;
		m_size = size;
		m_cells = std::vector<uint16_t>(static_cast<uint64_t>(size) * size * size);
		EnsureAsset(context->GetBlockAssetFromCodeName("empty"));
	}

	uint16_t GetPaletteIndex (BlockAsset* asset) {
		EnsureAsset(asset);
		return m_blockIdToPaletteIndex[asset->id];
	}

	void EnsureAsset (BlockAsset* asset) {
		auto iterator = m_blockIdToPaletteIndex.find(asset->id);

		// Make sure that if block is missing, it gets added to the palette
		if(iterator == m_blockIdToPaletteIndex.end()) {
			uint32_t paletteId = static_cast<uint32_t>(m_palette.size());
			m_blockIdToPaletteIndex[asset->id] = paletteId;
			m_palette.push_back(asset->id);
		}
	}

	void SetCell(uint8_t x, uint8_t y, uint8_t z, BlockAsset* asset) {
		EnsureAsset(asset);
		m_cells[x + y * static_cast<uint64_t>(m_size) + z * static_cast<uint64_t>(m_size) * m_size] = m_blockIdToPaletteIndex[asset->id];
	}

	void SetCellUnsafe(uint8_t x, uint8_t y, uint8_t z, BlockAsset* asset) {
		m_cells[x + y * static_cast<uint64_t>(m_size) + z * static_cast<uint64_t>(m_size) * m_size] = m_blockIdToPaletteIndex[asset->id];
	}

	void SetCellUnsafe(uint8_t x, uint8_t y, uint8_t z, uint16_t paletteIndex) {
		m_cells[x + y * static_cast<uint64_t>(m_size) + z * static_cast<uint64_t>(m_size) * m_size] = paletteIndex;
	}

	BlockAsset* GetCell(uint8_t x, uint8_t y, uint8_t z) {
		return m_context->GetBlockAssetFromIndex(m_palette[m_cells[
			x + y * static_cast<uint64_t>(m_size) + z * static_cast<uint64_t>(m_size) * m_size]]);
	}

	uint16_t GetCellPaletteIndex(uint8_t x, uint8_t y, uint8_t z) {
		return m_palette[m_cells[
			x + y * static_cast<uint64_t>(m_size) + z * static_cast<uint64_t>(m_size) * m_size]];
	}

	void FillCells (BlockAsset* asset) {
		EnsureAsset(asset);
		for(uint8_t x = 0; x < m_size; x++) {
			for(uint8_t y = 0; y < m_size; y++) {
				for(uint8_t z = 0; z < m_size; z++) {
					SetCell(x, y, z, asset);
				}
			}
		}
	}

	void FillCellsUnsafe (uint16_t paletteIndex) {
		for(int i = 0; i < m_size * m_size * m_size; i++) {
			m_cells[i] = paletteIndex;
		}
	}

	int GetPaletteSize () {
		return static_cast<int>(m_palette.size());
	}

	int size() {
		return m_size;
	}
};
