#pragma once
#include "Vec3.h"
#include <string>
#include <iostream>

inline float clamp(float c, float min, float max){
	if(c>max)
		return max;
	if(c<min)
		return min;
	return c;
}

namespace EPIC{

	class Color: public Vec3<float>{
		public:
		Color():Vec3<float>(0.0f, 0.0f, 0.0f){}
		Color(const Vec3<float>& v) : Vec3<float>(v){}
		Color(float r, float g, float b) : Vec3<float>(r, g, b){}
		Color(const std::string& hex) : Vec3<float>((float)std::stoi(hex.substr(0, 2), 0, 16)/256.0f,
													(float)std::stoi(hex.substr(2, 2), 0, 16)/256.0f,
													(float)std::stoi(hex.substr(4, 2), 0, 16)/256.0f){
		
		}

		// Written as an 8-bit pixel: gamma 2 (square root), then clamped to [0, 255]
		friend std::ostream& operator<< (std::ostream& out, const Color& v){

			float r = sqrt(v[0]);
			float g = sqrt(v[1]);
			float b = sqrt(v[2]);

			out<<static_cast<int>(255.0f*clamp(r, 0.0f, 1.0f))<<" "
			   <<static_cast<int>(255.0f*clamp(g, 0.0f, 1.0f))<<" "
			   <<static_cast<int>(255.0f*clamp(b, 0.0f, 1.0f))<<" ";
			return out;
		}
		
	};



}
