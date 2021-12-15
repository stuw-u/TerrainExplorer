#include "BlockAssetManager.h"

// Deserialise all block asset data from a text file.
BlockAssetManager::BlockAssetManager(const char* filepath) {

	// Creating file stream
	std::ifstream myfile; 
	std::string line;
	myfile.open(filepath);
	if(!myfile) {
		std::cerr << "Failed to load BlockList.txt" << std::endl;
		return;
	}

	// Create empty block (aka air)
	blockAssets.push_back(new BlockAsset());

	// Read the file line by line, and by creating a block asset once we
	// get all the data we need. dumpBlock should dump
	// the last blockAsset once all have been processed in case there is
	// no final line
	uint32_t currentId = 1;
	uint32_t textureIndex = 0;
	bool dumpBlock = false;
	BlockAsset* blockAsset;
	blockAsset = new BlockAsset();
	while(myfile.good()) {
		myfile >> line;

		// Dump blocks if a new one is starting or if there's an empty line.
		if(dumpBlock && (line.empty() || line.rfind("code_name:", 0) == 0)) {
			dumpBlock = false;
			blockAssets.push_back(blockAsset);
			blockAsset = new BlockAsset();
			currentId++;
		}
		
		// Fill in the parameters
		if(line.rfind("code_name:", 0) == 0) {
			dumpBlock = true; // Flag since we have a new blockAsset
			blockAsset->id = currentId;
			blockAsset->codeName = line.substr(10);
			codeNameToId[blockAsset->codeName] = blockAsset->id;
		}
		else if(line.rfind("texture_index:", 0) == 0) {
			blockAsset->textureIndex = std::stoi(line.substr(14));
		}
		else if(line.rfind("color_r:", 0) == 0) {
			blockAsset->colorR = std::stoi(line.substr(8));
		}
		else if(line.rfind("color_g:", 0) == 0) {
			blockAsset->colorG = std::stoi(line.substr(8));
		}
		else if(line.rfind("color_b:", 0) == 0) {
			blockAsset->colorB = std::stoi(line.substr(8));
		}
	}
	if(dumpBlock) blockAssets.push_back(blockAsset);

}

BlockAssetManager::~BlockAssetManager() {
	for(int i = 0; i < blockAssets.size(); i++) {
		delete blockAssets[i];
	}
}

BlockAsset* BlockAssetManager::GetBlockAssetFromCodeName(const char* codeName) const {
	return blockAssets[GetBlockIdFromCodeName(codeName)];
}

BlockAsset* BlockAssetManager::GetBlockAssetFromIndex(int index) const {
	return blockAssets[index];
}


uint32_t BlockAssetManager::GetBlockIdFromCodeName(const char* codeName) const {
	auto iterator = codeNameToId.find(codeName);
	if(iterator == codeNameToId.end()) {
		return 0;
	}
	else {
		return iterator->second;
	}
}