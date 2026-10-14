#pragma once
#include "Color.h"
#include <vector>

#define MAX_PIXEL_VALUE 255

namespace EPIC{

	// Maps a unit direction to a point (u, v) of an equirectangular image, both in [0, 1].
	// u turns around the vertical axis, v goes from 0 (straight up) to 1 (straight down).
	void directionToUV(const Vec3<float>& direction, float& u, float& v);

	class Image{

	public:
		Image(int width, int height);
		Image(const char* filepath);
		Image(const Image& img);
		std::shared_ptr<Color>& getPixel(int row, int col);
		// Pixel seen in a direction, when the image is used as an environment map
		std::shared_ptr<Color> sample(const Vec3<float>& direction);
		bool save(const char* filepath);

		int width() const { return m_width; }
		int height() const { return m_height; }

	private:
		int m_height;
		int m_width;
		std::vector<std::shared_ptr<Color>> m_pixels;
	};

}
