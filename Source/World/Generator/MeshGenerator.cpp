#include "MeshGenerator.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <array>

using ivec3 = glm::ivec3;
using vec3 = glm::vec3;
using std::vector;
using std::array;

const vec3 MeshGenerator::enumToVert[6][4]{
    {   // Pos_X
        vec3(1, 0, 1),
        vec3(1, 0, 0),
        vec3(1, 1, 0),
        vec3(1, 1, 1)
    },{ // Neg_X
        vec3(0, 0, 0),
        vec3(0, 0, 1),
        vec3(0, 1, 1),
        vec3(0, 1, 0)
    },{ // Pos_Y
        vec3(0, 1, 1),
        vec3(1, 1, 1),
        vec3(1, 1, 0),
        vec3(0, 1, 0)
    },{ // Neg_Y
        vec3(0, 0, 0),
        vec3(1, 0, 0),
        vec3(1, 0, 1),
        vec3(0, 0, 1)
    },{ // Pos_Z
        vec3(0, 0, 1),
        vec3(1, 0, 1),
        vec3(1, 1, 1),
        vec3(0, 1, 1)
    },{ // Neg_Z
        vec3(1, 0, 0),
        vec3(0, 0, 0),
        vec3(0, 1, 0),
        vec3(1, 1, 0)
    }
};

const vec3 MeshGenerator::enumToNormal[6]{
    vec3(1,0,0), vec3(-1,0,0),
    vec3(0,1,0), vec3(0,-1,0),
    vec3(0,0,1), vec3(0,0,-1)
};

const ivec3 MeshGenerator::enumToIntNormal[6]{
    ivec3(1,0,0),   // 0
    ivec3(-1,0,0),  // 1
    ivec3(0,1,0),   // 2
    ivec3(0,-1,0),  // 3
    ivec3(0,0,1),   // 4
    ivec3(0,0,-1)   // 5
};

void MeshGenerator::AddFace (
    vector<Chunk*>& chunks,
    ChunkMeshData& mdat, vec3 blockPos,
    FaceDirection dir, vec3 color) {

    ivec3 normal = enumToIntNormal[(int)dir];

    // Direction cases are the same as the enum => normal above
    array<ivec3, 6> adjacent;
    if((int)dir == 0) { adjacent = array<ivec3, 6>{ivec3( 0,-1, 0), ivec3( 0, 0, 1), ivec3( 0, 1, 0), ivec3( 0, 0,-1)}; } 
    if((int)dir == 1) { adjacent = array<ivec3, 6>{ivec3( 0,-1, 0), ivec3( 0, 0,-1), ivec3( 0, 1, 0), ivec3( 0, 0, 1)}; }
    if((int)dir == 2) { adjacent = array<ivec3, 6>{ivec3( 0, 0, 1), ivec3(-1, 0, 0), ivec3( 0, 0,-1), ivec3( 1, 0, 0)}; }
    if((int)dir == 3) { adjacent = array<ivec3, 6>{ivec3( 0, 0,-1), ivec3(-1, 0, 0), ivec3( 0, 0, 1), ivec3( 1, 0, 0)}; }
    if((int)dir == 4) { adjacent = array<ivec3, 6>{ivec3( 0,-1, 0), ivec3(-1, 0, 0), ivec3( 0, 1, 0), ivec3( 1, 0, 0)}; }
    if((int)dir == 5) { adjacent = array<ivec3, 6>{ivec3( 0,-1, 0), ivec3( 1, 0, 0), ivec3( 0, 1, 0), ivec3(-1, 0, 0)}; }

    float ao0 = CalculateAO(chunks, blockPos, adjacent[0], adjacent[1], normal);
    float ao1 = CalculateAO(chunks, blockPos, adjacent[3], adjacent[0], normal);
    float ao3 = CalculateAO(chunks, blockPos, adjacent[1], adjacent[2], normal);
    float ao2 = CalculateAO(chunks, blockPos, adjacent[2], adjacent[3], normal);

    VertexData vdat0, vdat1, vdat2, vdat3;
    vdat0.pos = enumToVert[(int)dir][0] + blockPos;
    vdat1.pos = enumToVert[(int)dir][1] + blockPos;
    vdat2.pos = enumToVert[(int)dir][2] + blockPos;
    vdat3.pos = enumToVert[(int)dir][3] + blockPos;
    vdat0.normal = normal;
    vdat1.normal = normal;
    vdat2.normal = normal;
    vdat3.normal = normal;
    vdat0.color = color;
    vdat1.color = color;
    vdat2.color = color;
    vdat3.color = color;
    vdat0.ao = ao0;
    vdat1.ao = ao1;
    vdat2.ao = ao2;
    vdat3.ao = ao3;
    mdat.verticies.push_back(vdat0);
    mdat.verticies.push_back(vdat1);
    mdat.verticies.push_back(vdat2);
    mdat.verticies.push_back(vdat3);
    mdat.indicies.push_back(mdat.indiciesCount + 0);
    mdat.indicies.push_back(mdat.indiciesCount + 1);
    mdat.indicies.push_back(mdat.indiciesCount + 2);
    mdat.indicies.push_back(mdat.indiciesCount + 2);
    mdat.indicies.push_back(mdat.indiciesCount + 3);
    mdat.indicies.push_back(mdat.indiciesCount + 0);
    mdat.indiciesCount += 4;
}

void MeshGenerator::GenerateChunkMesh (Chunk* chunk, vector<Chunk*>& chunks) {
	Mesh mesh;
    chunk->meshData.Reserve();

	// Fill up the chunk mesh data
    for(uint8_t z = 0; z < 16; z++) {
        for(uint8_t y = 0; y < 16; y++) {
            for(uint8_t x = 0; x < 16; x++) {
                BlockAsset* centerAsset = chunk->data.GetCell(x, y, z);
                if(centerAsset->id == 0)
                    continue;
                vec3 color = vec3(
                    centerAsset->colorR / (float)255,
                    centerAsset->colorG / (float)255,
                    centerAsset->colorB / (float)255);
                for(int i = 0; i < 6; i++) {
                    vec3 normal = enumToNormal[i];
                    ivec3 blockPos = ivec3(x + normal.x, y + normal.y, z + normal.z);

                    int id = GetPaletteIndexInChunks(blockPos, chunks);
                    if(id == 0) {
                        AddFace(chunks, chunk->meshData, vec3(x, y, z), (FaceDirection)i, color);
                    }
                }
            }
        }
    }

    //std::cout << "Index buffer size: " << mdata.indicies.size() << std::endl;
    //std::cout << "Verts buffer size: " << mdata.verticies.size() << std::endl;
    

	// Send it all to the gpu
    //mesh.SetData(mdata.verticies, mdata.indicies);
}

BlockAsset* MeshGenerator::GetCellInChunks (ivec3 blockPos, vector<Chunk*>& chunks) {
    int size = chunks[13]->data.size(); // 13 is the middle chunk. It's guarenteed that it exists
    int x = blockPos.x,
        y = blockPos.y,
        z = blockPos.z;
    int chunkX = floorl(x / (double)size),
        chunkY = floorl(y / (double)size),
        chunkZ = floorl(z / (double)size);
    x -= chunkX * size;
    y -= chunkY * size;
    z -= chunkZ * size;
    int index = (chunkX + 1) + (chunkY + 1) * 3 + (chunkZ + 1) * 9;
    if(chunks[index] == nullptr)
        return nullptr;
    return chunks[index]->data.GetCell(x, y, z);
}

int MeshGenerator::GetPaletteIndexInChunks (ivec3 blockPos, vector<Chunk*>& chunks) {
    int size = chunks[13]->data.size(); // 13 is the middle chunk. It's guarenteed that it exists
    int x = blockPos.x, 
        y = blockPos.y, 
        z = blockPos.z;
    int chunkX = floorl(x / (double)size),
        chunkY = floorl(y / (double)size),
        chunkZ = floorl(z / (double)size);
    x -= chunkX * size;
    y -= chunkY * size;
    z -= chunkZ * size;
    int index = (chunkX + 1) + (chunkY + 1) * 3 + (chunkZ + 1) * 9;
    if(chunks[index] == nullptr)
        return 0;
    return chunks[index]->data.GetCellPaletteIndex(x, y, z);
}

float MeshGenerator::CalculateAO (
    vector<Chunk*>& chunks, ivec3 p, 
    ivec3 left, ivec3 right, ivec3 up) 
{
    int side1  = GetPaletteIndexInChunks(p + left  + up,         chunks) == 0 ? 0 : 1;
    int side2  = GetPaletteIndexInChunks(p + right + up,         chunks) == 0 ? 0 : 1;
    int corner = GetPaletteIndexInChunks(p + left  + right + up, chunks) == 0 ? 0 : 1;

    float ao = (side1 == 1 && side2 == 1) ? 0 : (3 - (side1 + side2 + corner));
    return ao * 0.33333333333f;
}
