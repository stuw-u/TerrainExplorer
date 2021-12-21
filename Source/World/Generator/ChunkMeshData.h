#pragma once
#include <vector>
#include <cstdint>

constexpr size_t PREALLOC_VERTS = 73728;
constexpr size_t PREALLOC_INDICIES = 49152;

struct ChunkMeshData {
    std::vector<VertexData> verticies; // Could be optimized further? (Flat)
    std::vector<uint32_t> indicies;
    uint32_t indiciesCount = 0;
    bool isAllocated;

    ChunkMeshData() {}

    void Reserve() {
        verticies.reserve(PREALLOC_VERTS);
        indicies.reserve(PREALLOC_INDICIES);
    }

    void Dispose () {
        isAllocated = false;
        verticies = std::vector<VertexData>();
        indicies = std::vector<uint32_t>();
    }
};