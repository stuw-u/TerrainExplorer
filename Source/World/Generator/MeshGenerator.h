#pragma once
#include "../../Engine/Mesh.h"
#include "../Chunk/Chunk.h"
#include "../../External/glm/vec3.hpp"
#include <vector>

constexpr size_t PREALLOC_VERTS = 73728;
constexpr size_t PREALLOC_INDICIES = 49152;

enum class FaceDirection {Pos_X, Neg_X, Pos_Y, Neg_Y, Pos_Z, Neg_Z};


struct ChunkMeshData {
	std::vector<VertexData> verticies; // Could be optimized further? (Flat)
	std::vector<uint32_t> indicies;
    uint32_t indiciesCount = 0;

    ChunkMeshData() {
        verticies.reserve(PREALLOC_VERTS);
        indicies.reserve(PREALLOC_INDICIES);
    }
};

class MeshGenerator {

    static const glm::vec3 enumToVert[6][4];
    static const glm::vec3 enumToNormal[6];

    void AddFace (ChunkMeshData& mdat, glm::vec3 blockPos, FaceDirection dir, glm::vec3 color);


public:
    static const glm::ivec3 enumToIntNormal[6];

    Mesh GenerateChunkMesh(Chunk* chunk, std::vector<Chunk*>& chunks);
    
    static BlockAsset* GetCellInChunks (glm::ivec3 blockPos, std::vector<Chunk*>& chunks);
};