#include "Image.h"
#include <fstream>
#include "Vec3.h"
#include "Color.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iomanip>
#include <stdexcept>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

namespace EPIC{

	Image::Image(int width, int height):m_width(width),
		                                m_height(height),
		                                m_pixels(width*height){}

	Image::Image(const Image& img):m_height(img.m_height),
	                               m_width(img.m_width),
	                               m_pixels(img.m_pixels){}

	Image::Image(const char* filepath){
		int components;
		auto imageData = stbi_load(filepath,
								   &m_width, &m_height, &components, STBI_rgb);

		if(imageData == nullptr){
			throw std::runtime_error(std::string("Could not load image: ")+filepath);
		}

		m_pixels.resize(m_width*m_height);

		#pragma omp parallel for
		for(int i = 0; i<m_height; i++){
			for(int j = 0; j<m_width; j++){
				float r = (float)imageData[(i*m_width+j)*3 + 0]/255.0;
				float g = (float)imageData[(i*m_width+j)*3 + 1]/255.0;
				float b = (float)imageData[(i*m_width+j)*3 + 2]/255.0;
				m_pixels[i*m_width+j] = std::make_shared<Color>(r, g, b);
			}
		}

		stbi_image_free(imageData);
	}
	bool Image::save(const char* filepath){
		std::cout<<"Saving : "<<filepath<<std::endl;
		std::ofstream file(filepath);

		file<<"P3"<<std::endl;
		file<<m_width << " " << m_height <<std::endl;
		file<<MAX_PIXEL_VALUE<<std::endl;


		for(auto c:m_pixels){
			file<<*c<<std::endl;
		}

		return true;
	}



	std::shared_ptr<Color>& Image::getPixel(int row, int col){
		if(m_width*row+col < m_width*m_height)
			return m_pixels[m_width*row+col];
		return  m_pixels[0];
	}


	void directionToUV(const Vec3<float>& direction, float& u, float& v){
		auto phi = atan2(-direction[2], direction[0]) + M_PI;
		auto theta = acos(clamp(-direction[1], -1.0f, 1.0f));

		u = clamp(phi/(2*M_PI), 0.0f, 1.0f);
		v = 1.0f-clamp(theta/M_PI, 0.0f, 1.0f);
	}


	std::shared_ptr<Color> Image::sample(const Vec3<float>& direction){
		float u, v;
		directionToUV(direction, u, v);

		// u = 1 and v = 1 belong to the last column and the last row
		int row = (int)floor(v*m_height);
		int col = (int)floor(u*m_width);
		if(row > m_height-1) row = m_height-1;
		if(col > m_width-1) col = m_width-1;

		return m_pixels[m_width*row+col];
	}
}
