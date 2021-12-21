#pragma once
#include <iostream>
#include <chrono>

class DebugChrono {
	std::chrono::steady_clock::time_point begin;
public:
	DebugChrono() { }

	void Start () {
		begin = std::chrono::steady_clock::now();
	}

	void StopAndPrint(const char* message, int iterations) {
		if(iterations == 0) iterations = 1;

		auto end = std::chrono::steady_clock::now();
		auto durationChunk = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		//std::cout << message << (durationChunk / iterations) << "[microseconds]" << std::endl;
	}
	
};
