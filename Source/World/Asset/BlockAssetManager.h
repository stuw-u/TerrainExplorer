#pragma once
#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include "BlockAsset.h"

class BlockAssetManager {
private:
	std::vector<BlockAsset*> blockAssets;
	std::map<std::string, int> codeNameToId;
public:
	BlockAssetManager(const char* filepath);
	~BlockAssetManager();

	BlockAsset* GetBlockAssetFromCodeName(const char* codeName) const;
	BlockAsset* GetBlockAssetFromIndex(int index) const;
	uint32_t GetBlockIdFromCodeName(const char* codeName) const;
	
};