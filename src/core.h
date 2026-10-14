#pragma once
#include <random>

namespace EPIC{

	// Defaults for a render, overridable from the command line (see main.cpp)
	struct RenderSettings{
		int width = 1920;
		int height = 1080;
		int samples_per_pixel = 100;
		int max_depth = 20;
	};

}

// One generator per thread: a std::mt19937 shared between OpenMP threads is a data race
inline std::mt19937& random_generator(){
	static thread_local std::mt19937 generator;
	return generator;
}

// Give every thread its own sequence, call it once at the start of each thread
inline void seed_random(unsigned int seed){
	random_generator().seed(seed);
}

inline float random_double(){
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	return distribution(random_generator());
}

inline float random_double(float min, float max){
	return min+(max-min)*random_double();
}
