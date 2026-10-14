#include "doctest.h"
#include "Vec3.h"
#include "Color.h"
#include "core.h"
#include <sstream>

using EPIC::Vec3;

TEST_CASE("Vec3 arithmetic"){
	Vec3<float> a(1.0f, 2.0f, 3.0f);
	Vec3<float> b(4.0f, -5.0f, 6.0f);

	auto sum = a+b;
	CHECK((*sum)[0] == doctest::Approx(5.0f));
	CHECK((*sum)[1] == doctest::Approx(-3.0f));
	CHECK((*sum)[2] == doctest::Approx(9.0f));

	auto diff = a-b;
	CHECK((*diff)[0] == doctest::Approx(-3.0f));
	CHECK((*diff)[1] == doctest::Approx(7.0f));
	CHECK((*diff)[2] == doctest::Approx(-3.0f));

	auto scaled = a*2.0f;
	CHECK((*scaled)[2] == doctest::Approx(6.0f));

	auto product = a*b;
	CHECK((*product)[1] == doctest::Approx(-10.0f));

	CHECK(a.dot(b) == doctest::Approx(12.0f));
}

TEST_CASE("Vec3 cross product follows the right-hand rule"){
	Vec3<float> x(1.0f, 0.0f, 0.0f);
	Vec3<float> y(0.0f, 1.0f, 0.0f);

	auto z = x.cross(y);
	CHECK((*z)[0] == doctest::Approx(0.0f));
	CHECK((*z)[1] == doctest::Approx(0.0f));
	CHECK((*z)[2] == doctest::Approx(1.0f));

	auto minus_z = y.cross(x);
	CHECK((*minus_z)[2] == doctest::Approx(-1.0f));
}

TEST_CASE("Vec3 norm and normalize"){
	Vec3<float> v(3.0f, 4.0f, 12.0f);
	CHECK(v.norm() == doctest::Approx(13.0f));

	auto unit = v.normalize();
	CHECK(unit->norm() == doctest::Approx(1.0f));
	CHECK((*unit)[0] == doctest::Approx(3.0f/13.0f));
}

TEST_CASE("Vec3 reflect mirrors a direction about a normal"){
	Vec3<float> in(1.0f, -1.0f, 0.0f);
	Vec3<float> normal(0.0f, 1.0f, 0.0f);

	auto out = Vec3<float>::reflect(in, normal);
	CHECK((*out)[0] == doctest::Approx(1.0f));
	CHECK((*out)[1] == doctest::Approx(1.0f));
	CHECK((*out)[2] == doctest::Approx(0.0f));
}

TEST_CASE("random_unit_vector has length one"){
	seed_random(1);
	for(int k = 0; k<1000; k++){
		CHECK(Vec3<float>::random_unit_vector()->norm() == doctest::Approx(1.0f).epsilon(0.001));
	}
}

TEST_CASE("random_double stays in [0, 1) and repeats for the same seed"){
	seed_random(7);
	float first[5];
	for(int k = 0; k<5; k++){
		first[k] = random_double();
		CHECK(first[k] >= 0.0f);
		CHECK(first[k] <= 1.0f);
	}

	seed_random(7);
	for(int k = 0; k<5; k++){
		CHECK(random_double() == first[k]);
	}

	seed_random(8);
	CHECK(random_double() != first[0]);
}

TEST_CASE("Color from a hex string"){
	EPIC::Color grey("808080");
	CHECK(grey[0] == doctest::Approx(0.5f));
	CHECK(grey[1] == doctest::Approx(0.5f));
	CHECK(grey[2] == doctest::Approx(0.5f));

	EPIC::Color red("FF0000");
	CHECK(red[0] == doctest::Approx(255.0f/256.0f));
	CHECK(red[1] == doctest::Approx(0.0f));
}

TEST_CASE("Color is written gamma corrected and clamped"){
	std::ostringstream out;
	out<<EPIC::Color(0.25f, 1.0f, 4.0f);
	// sqrt(0.25) = 0.5 -> 127, sqrt(1) = 1 -> 255, sqrt(4) = 2 clamped to 1 -> 255
	CHECK(out.str() == "127 255 255 ");
}
