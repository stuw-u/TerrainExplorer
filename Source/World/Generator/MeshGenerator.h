#pragma once
#include "../../Engine/Mesh.h"
#include "../Chunk/Chunk.h"
#include "../../External/glm/vec3.hpp"
#include <vector>
#define BLOCK_SIZE 1.0f

enum class FaceDirection {Pos_X, Neg_X, Pos_Y, Neg_Y, Pos_Z, Neg_Z};

struct ChunkMeshData {
	std::vector<VertexData> verticies; // Could be optimized further by a lot (Flat)
	std::vector<uint32_t> indicies;
    uint32_t indiciesCount = 0;
};

class MeshGenerator {

    static const glm::vec3 enumToVert[6][4];
    static const glm::vec3 enumToNormal[6];

    void AddFace (ChunkMeshData& mdat, glm::vec3 blockPos, FaceDirection dir);

public:
    Mesh GenerateChunkMesh(Chunk* chunk);
};