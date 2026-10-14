#include "doctest.h"
#include "Image.h"
#include <cstdio>
#include <fstream>
#include <string>

using EPIC::Vec3;

TEST_CASE("directionToUV: up is the top row, down the bottom row, the horizon the middle"){
	float u, v;

	EPIC::directionToUV(Vec3<float>(0.0f, 1.0f, 0.0f), u, v);
	CHECK(v == doctest::Approx(0.0f));

	EPIC::directionToUV(Vec3<float>(0.0f, -1.0f, 0.0f), u, v);
	CHECK(v == doctest::Approx(1.0f));

	EPIC::directionToUV(Vec3<float>(1.0f, 0.0f, 0.0f), u, v);
	CHECK(v == doctest::Approx(0.5f));
}

TEST_CASE("directionToUV: turning around the vertical axis moves across the image"){
	float u, v;

	EPIC::directionToUV(Vec3<float>(0.0f, 0.0f, 1.0f), u, v);
	CHECK(u == doctest::Approx(0.25f));

	EPIC::directionToUV(Vec3<float>(1.0f, 0.0f, 0.0f), u, v);
	CHECK(u == doctest::Approx(0.5f));

	EPIC::directionToUV(Vec3<float>(0.0f, 0.0f, -1.0f), u, v);
	CHECK(u == doctest::Approx(0.75f));
}

TEST_CASE("Image::sample uses the real size of the image"){
	// 8 columns, 4 rows, each pixel coloured (row, column, 0)
	EPIC::Image sky(8, 4);
	for(int row = 0; row<4; row++){
		for(int col = 0; col<8; col++){
			sky.getPixel(row, col) = std::make_shared<EPIC::Color>((float)row, (float)col, 0.0f);
		}
	}

	auto up = sky.sample(Vec3<float>(0.0f, 1.0f, 0.0f));
	CHECK((*up)[0] == doctest::Approx(0.0f));

	// straight down is v = 1: the last row, not one past it
	auto down = sky.sample(Vec3<float>(0.0f, -1.0f, 0.0f));
	CHECK((*down)[0] == doctest::Approx(3.0f));

	// just below the horizon along +z: third row, u = 0.25 -> column 2
	auto low = sky.sample(*Vec3<float>(0.0f, -0.1f, 1.0f).normalize());
	CHECK((*low)[0] == doctest::Approx(2.0f));
	CHECK((*low)[1] == doctest::Approx(2.0f));

	// just above the horizon along -z: second row, u = 0.75 -> column 6
	auto high = sky.sample(*Vec3<float>(0.0f, 0.1f, -1.0f).normalize());
	CHECK((*high)[0] == doctest::Approx(1.0f));
	CHECK((*high)[1] == doctest::Approx(6.0f));
}

TEST_CASE("Image::save writes a plain PPM"){
	EPIC::Image img(2, 1);
	img.getPixel(0, 0) = std::make_shared<EPIC::Color>(1.0f, 0.0f, 0.0f);
	img.getPixel(0, 1) = std::make_shared<EPIC::Color>(0.25f, 0.25f, 0.25f);

	const char* path = "epic_test_save.ppm";
	REQUIRE(img.save(path));

	std::ifstream file(path);
	std::string magic;
	int width, height, max, r, g, b;
	file>>magic>>width>>height>>max;
	CHECK(magic == "P3");
	CHECK(width == 2);
	CHECK(height == 1);
	CHECK(max == 255);

	file>>r>>g>>b;
	CHECK(r == 255);
	CHECK(g == 0);
	file>>r>>g>>b;
	CHECK(r == 127);
	CHECK(b == 127);

	file.close();
	std::remove(path);
}

TEST_CASE("Loading a missing image fails with an error, not a crash"){
	CHECK_THROWS_AS(EPIC::Image("no_such_file.hdr"), std::runtime_error);
}
