#include "doctest.h"
#include "Camera.h"

using EPIC::Vec3;

// Camera at the origin looking down -z, 90 degree field of view, square 100x100 image
static EPIC::Camera makeCamera(float aperture, float focus_dist){
	return EPIC::Camera(Vec3<float>(0.0f, 0.0f, 0.0f),
						Vec3<float>(0.0f, 0.0f, -1.0f),
						Vec3<float>(0.0f, 1.0f, 0.0f),
						90.0f, 1.0f, aperture, focus_dist, 100, 100);
}

TEST_CASE("With a pinhole every ray starts at the camera position"){
	auto camera = makeCamera(0.0f, 1.0f);
	seed_random(1);

	for(int k = 0; k<100; k++){
		auto ray = camera.getRay(k, 99-k);
		CHECK(ray->origin()->norm() == doctest::Approx(0.0f));
	}
}

TEST_CASE("The centre pixel looks at the target, the corners look away from it"){
	auto camera = makeCamera(0.0f, 1.0f);
	seed_random(1);

	auto centre = camera.getRay(50, 50);
	CHECK((*centre->direction())[2] < -0.999f);

	// row 0, column 0 is the lower left corner: 45 degrees off axis in x and in y
	auto corner = camera.getRay(0, 0);
	CHECK((*corner->direction())[0] < -0.55f);
	CHECK((*corner->direction())[1] < -0.55f);
	CHECK((*corner->direction())[2] < 0.0f);

	auto opposite = camera.getRay(99, 99);
	CHECK((*opposite->direction())[0] > 0.55f);
	CHECK((*opposite->direction())[1] > 0.55f);
}

TEST_CASE("Ray directions are unit vectors"){
	auto camera = makeCamera(0.5f, 3.0f);
	seed_random(1);

	for(int k = 0; k<100; k++){
		CHECK(camera.getRay(k, k)->direction()->norm() == doctest::Approx(1.0f));
	}
}

TEST_CASE("With a lens, rays start on a disk of radius aperture/2 around the camera"){
	auto camera = makeCamera(0.5f, 2.0f);
	seed_random(2);

	float farthest = 0.0f;
	for(int k = 0; k<2000; k++){
		auto ray = camera.getRay(50, 50);
		auto o = ray->origin();
		CHECK((*o)[2] == doctest::Approx(0.0f));
		CHECK(o->norm() <= 0.25f);
		if(o->norm()>farthest) farthest = o->norm();
	}
	// the disk is really used, not only its centre
	CHECK(farthest > 0.2f);
}

TEST_CASE("Rays of one pixel meet again on the focus plane"){
	// Focus plane at z = -2 spans x, y in [-2, 2]: pixel 50 of 100 covers [0, 0.04]
	auto camera = makeCamera(0.5f, 2.0f);
	seed_random(5);

	for(int k = 0; k<2000; k++){
		auto ray = camera.getRay(50, 50);
		float t = (-2.0f-(*ray->origin())[2])/(*ray->direction())[2];
		auto p = ray->pointAt(t);
		CHECK((*p)[0] >= -0.001f);
		CHECK((*p)[0] <= 0.041f);
		CHECK((*p)[1] >= -0.001f);
		CHECK((*p)[1] <= 0.041f);
	}
}

TEST_CASE("The focus plane is as wide as the field of view at the focus distance"){
	// Same camera: row 80, column 20 covers x in [-1.2, -1.16] and y in [1.2, 1.24] at z = -2
	auto camera = makeCamera(0.5f, 2.0f);
	seed_random(6);

	for(int k = 0; k<2000; k++){
		auto ray = camera.getRay(80, 20);
		float t = (-2.0f-(*ray->origin())[2])/(*ray->direction())[2];
		auto p = ray->pointAt(t);
		CHECK((*p)[0] >= -1.201f);
		CHECK((*p)[0] <= -1.159f);
		CHECK((*p)[1] >= 1.199f);
		CHECK((*p)[1] <= 1.241f);
	}
}
