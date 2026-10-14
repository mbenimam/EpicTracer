#include "doctest.h"
#include "Material.h"
#include "Sphere.h"

using EPIC::Vec3;

// A hit at `position` on a surface whose outward normal is +y.
// Sphere::hit stores the inward normal, so the record holds -y.
static EPIC::HitRecord hitFromAbove(float x, float y, float z){
	EPIC::HitRecord rec;
	rec.position = std::make_shared<Vec3<float>>(x, y, z);
	rec.normal = std::make_shared<Vec3<float>>(0.0f, -1.0f, 0.0f);
	rec.t = 1.0f;
	return rec;
}

static EPIC::Ray rayDown45(){
	return EPIC::Ray(std::make_shared<Vec3<float>>(-1.0f, 1.0f, 0.0f),
					 std::make_shared<Vec3<float>>(1.0f, -1.0f, 0.0f));
}

TEST_CASE("Metal reflects like a mirror and keeps its colour"){
	EPIC::Metal metal(EPIC::Color(0.8f, 0.6f, 0.2f));
	auto rec = hitFromAbove(0.0f, 0.0f, 0.0f);
	EPIC::Color attenuation;
	EPIC::Ray out;

	REQUIRE(metal.scatter(rayDown45(), rec, attenuation, out));

	float s = 1.0f/sqrt(2.0f);
	CHECK((*out.direction())[0] == doctest::Approx(s));
	CHECK((*out.direction())[1] == doctest::Approx(s));
	CHECK((*out.direction())[2] == doctest::Approx(0.0f));
	CHECK((*out.origin())[1] == doctest::Approx(0.0f));

	CHECK(attenuation[0] == doctest::Approx(0.8f));
	CHECK(attenuation[1] == doctest::Approx(0.6f));
	CHECK(attenuation[2] == doctest::Approx(0.2f));
}

TEST_CASE("Lambertian scatters above the surface, wherever the hit point is"){
	EPIC::Lambertian diffuse(EPIC::Color(0.5f, 0.5f, 0.5f));
	EPIC::Color attenuation;

	// Far from the world origin on purpose: the direction must not depend on the position
	float positions[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, -50.0f, 0.0f}, {40.0f, -3.0f, -70.0f}};

	seed_random(3);
	for(auto& p : positions){
		auto rec = hitFromAbove(p[0], p[1], p[2]);
		for(int k = 0; k<500; k++){
			EPIC::Ray out;
			REQUIRE(diffuse.scatter(rayDown45(), rec, attenuation, out));
			// outward normal is +y
			CHECK((*out.direction())[1] >= 0.0f);
			CHECK((*out.origin())[0] == doctest::Approx(p[0]));
			CHECK((*out.origin())[1] == doctest::Approx(p[1]));
		}
	}

	CHECK(attenuation[0] == doctest::Approx(0.5f));
}

TEST_CASE("Lambertian scattering is centred on the normal"){
	EPIC::Lambertian diffuse(EPIC::Color(0.5f, 0.5f, 0.5f));
	EPIC::Color attenuation;
	auto rec = hitFromAbove(0.0f, -50.0f, 0.0f);

	seed_random(4);
	float sum[3] = {0.0f, 0.0f, 0.0f};
	int n = 20000;
	for(int k = 0; k<n; k++){
		EPIC::Ray out;
		diffuse.scatter(rayDown45(), rec, attenuation, out);
		for(int a = 0; a<3; a++) sum[a] += (*out.direction())[a];
	}

	// The mean direction of a cosine-weighted lobe is 2/3 along the normal
	CHECK(sum[0]/n == doctest::Approx(0.0f).epsilon(0.02));
	CHECK(sum[1]/n == doctest::Approx(2.0f/3.0f).epsilon(0.02));
	CHECK(sum[2]/n == doctest::Approx(0.0f).epsilon(0.02));
}

TEST_CASE("A light emits its colour and is flagged as a light"){
	EPIC::LightMat light(EPIC::Color(1.0f, 0.9f, 0.8f));
	auto rec = hitFromAbove(0.0f, 0.0f, 0.0f);
	EPIC::Color attenuation;
	EPIC::Ray out;

	CHECK(light.type == EPIC::LIGHT_MATERIAL);
	REQUIRE(light.scatter(rayDown45(), rec, attenuation, out));
	CHECK(attenuation[1] == doctest::Approx(0.9f));

	EPIC::Metal metal(EPIC::Color(1.0f, 1.0f, 1.0f));
	EPIC::Lambertian diffuse(EPIC::Color(1.0f, 1.0f, 1.0f));
	CHECK(metal.type == EPIC::NOT_LIGHT_MATERIAL);
	CHECK(diffuse.type == EPIC::NOT_LIGHT_MATERIAL);
}
