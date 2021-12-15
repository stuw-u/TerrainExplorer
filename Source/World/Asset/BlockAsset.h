#pragma once
#include <cstdint>
#include <string>

// Block asset with id 0 reserved for air/empty.
struct BlockAsset {
	uint32_t id = 0;
	std::string codeName = "empty";
	uint16_t textureIndex = 0;

	uint8_t colorR = 0;
	uint8_t colorG = 0;
	uint8_t colorB = 0;
};