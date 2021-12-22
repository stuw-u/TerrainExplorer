#pragma once
#include "../../../External/FastNoiseLite/FastNoiseLite.h"
#include "../Utils/TerrainUtils.h"
#include "INoiseFunction.h"
#include <array>

struct YosemiteValley_Context : INoiseFunction {
	// Noise samplers
	FastNoiseLite baseNoise;
	FastNoiseLite cellNoise;
	std::array<FastNoiseLite, 8i32> offsets;

	YosemiteValley_Context() {
		baseNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		baseNoise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
		baseNoise.SetFractalOctaves(7);
		baseNoise.SetFractalGain(0.65f);
		baseNoise.SetFrequency(0.002f);
		cellNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
		cellNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
		cellNoise.SetFrequency(0.02f);
		for(int i = 0; i < 8; i++) {
			FastNoiseLite noiseOffset;
			noiseOffset.SetSeed(i + 1);
			noiseOffset.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
			noiseOffset.SetFractalType(FastNoiseLite::FractalType_FBm);
			noiseOffset.SetFractalOctaves(1);
			offsets[i] = noiseOffset;
		}
		for(int i = 0; i < 3; i++) {
			offsets[i * 2].SetFrequency(0.002f);
			offsets[i * 2 + 1].SetFrequency(0.006f);
		}
		for(size_t i = 6; i < 8; i++) {
			offsets[i].SetFractalOctaves(3);
			offsets[i].SetFrequency(0.005f);
		}
	}

	float Sample (double x, double y, double z) {

		float beachFade = TUtils::invLerpClamp(y, 0, 30);

		float offsetX = offsets[6].GetNoise(x, y, z) * 0.4,
		offsetZ = offsets[7].GetNoise(x, y, z) * 0.4;

		float value = (
			(baseNoise.GetNoise(x + offsetX, z + offsetZ) - 0.8) +
			(1.0 - cellNoise.GetNoise(x + offsetX * 0.5, z + offsetZ * 0.5)) * 1.1) * 50;


		for(int step = 0; step < 3; step++) {
			value = TUtils::lerp(beachFade,
				value,
				TUtils::terrace(
					value,
					TUtils::lerp(offsets[step * 2].GetNoise(x, y, z), 30, 50) * (1.0 - step * 0.2),
					TUtils::lerp(offsets[step * 2 + 1].GetNoise(x, y, z) + step * 0.33, 0, 1),
					0.0
				));
		}

		return value;
	}
};

struct Moon_Context : INoiseFunction{
	FastNoiseLite baseNoise;
	FastNoiseLite cellNoise;

	Moon_Context() {
		baseNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		baseNoise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
		baseNoise.SetFractalOctaves(6);
		baseNoise.SetFractalGain(0.5f);
		baseNoise.SetFrequency(0.001f);

		cellNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
		cellNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean);
		cellNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
	}

	float Sample (double x, double y, double z) {
		float crater = 0.0f;
		float freq = 0.005f;
		float ampl = 1.0f;
		for(int i = 0; i < 3; i++) {
			cellNoise.SetFrequency(freq);
			cellNoise.SetSeed(i);
			crater += TUtils::craterDepth(cellNoise.GetNoise(x, z) + 1.0f, 16.0f, 0.3f, 1.0f) * ampl;
			freq *= 2.0f;
			ampl *= 0.5f;
		}

		float value = baseNoise.GetNoise(x, z) * 30.0f + crater * 10.0f - 40.0f;

		return value;
	}
};