#include "doctest.h"
#include "Sphere.h"
#include "Material.h"

using EPIC::Vec3;

static std::shared_ptr<EPIC::Ray> makeRay(float ox, float oy, float oz, float dx, float dy, float dz){
	return std::make_shared<EPIC::Ray>(std::make_shared<Vec3<float>>(ox, oy, oz),
									   std::make_shared<Vec3<float>>(dx, dy, dz));
}

static std::shared_ptr<EPIC::Sphere> makeSphere(float x, float y, float z, float radius){
	auto material = std::make_shared<EPIC::Lambertian>(EPIC::Color("808080"));
	return std::make_shared<EPIC::Sphere>(std::make_shared<Vec3<float>>(x, y, z), radius, material);
}

TEST_CASE("A ray aimed at a sphere hits its near side"){
	auto sphere = makeSphere(0.0f, 0.0f, 0.0f, 1.0f);
	EPIC::HitRecord rec;

	REQUIRE(sphere->hit(makeRay(0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f), 0.001f, 1000.0f, rec));
	CHECK(rec.t == doctest::Approx(4.0f));
	CHECK((*rec.position)[2] == doctest::Approx(-1.0f));
	// The stored normal points from the hit point towards the centre
	CHECK((*rec.normal)[0] == doctest::Approx(0.0f));
	CHECK((*rec.normal)[2] == doctest::Approx(1.0f));
	CHECK(rec.normal->norm() == doctest::Approx(1.0f));
}

TEST_CASE("The hit distance does not depend on the length of the direction"){
	auto sphere = makeSphere(0.0f, 0.0f, 0.0f, 1.0f);
	EPIC::HitRecord rec;

	REQUIRE(sphere->hit(makeRay(0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 10.0f), 0.001f, 1000.0f, rec));
	CHECK(rec.t == doctest::Approx(4.0f));
}

TEST_CASE("A ray that passes beside a sphere misses it"){
	auto sphere = makeSphere(0.0f, 0.0f, 0.0f, 1.0f);
	EPIC::HitRecord rec;

	CHECK_FALSE(sphere->hit(makeRay(2.0f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f), 0.001f, 1000.0f, rec));
	CHECK_FALSE(sphere->hit(makeRay(1.01f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f), 0.001f, 1000.0f, rec));
}

TEST_CASE("A sphere behind the ray is not hit"){
	auto sphere = makeSphere(0.0f, 0.0f, 0.0f, 1.0f);
	EPIC::HitRecord rec;

	CHECK_FALSE(sphere->hit(makeRay(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f), 0.001f, 1000.0f, rec));
}

TEST_CASE("From inside a sphere the far root is used"){
	auto sphere = makeSphere(0.0f, 0.0f, 0.0f, 2.0f);
	EPIC::HitRecord rec;

	REQUIRE(sphere->hit(makeRay(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f), 0.001f, 1000.0f, rec));
	CHECK(rec.t == doctest::Approx(2.0f));
	CHECK((*rec.position)[0] == doctest::Approx(2.0f));
}

TEST_CASE("Hits outside [t_min, t_max] are rejected"){
	auto sphere = makeSphere(0.0f, 0.0f, 0.0f, 1.0f);
	EPIC::HitRecord rec;

	// roots are 4 and 6
	CHECK_FALSE(sphere->hit(makeRay(0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f), 0.001f, 3.0f, rec));
	CHECK_FALSE(sphere->hit(makeRay(0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f), 7.0f, 1000.0f, rec));

	REQUIRE(sphere->hit(makeRay(0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f), 5.0f, 1000.0f, rec));
	CHECK(rec.t == doctest::Approx(6.0f));
}

TEST_CASE("HitList keeps the closest hit whatever the insertion order"){
	auto near = makeSphere(0.0f, 0.0f, 0.0f, 1.0f);
	auto far = makeSphere(0.0f, 0.0f, 10.0f, 1.0f);
	auto ray = makeRay(0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f);

	EPIC::HitList near_first;
	near_first.add(near);
	near_first.add(far);

	EPIC::HitList far_first;
	far_first.add(far);
	far_first.add(near);

	EPIC::HitRecord rec;
	REQUIRE(near_first.hit(ray, 0.001f, 1000.0f, rec));
	CHECK(rec.t == doctest::Approx(4.0f));

	EPIC::HitRecord rec2;
	REQUIRE(far_first.hit(ray, 0.001f, 1000.0f, rec2));
	CHECK(rec2.t == doctest::Approx(4.0f));
}

TEST_CASE("An empty HitList is never hit"){
	EPIC::HitList world;
	EPIC::HitRecord rec;
	CHECK_FALSE(world.hit(makeRay(0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f), 0.001f, 1000.0f, rec));
}
