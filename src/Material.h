#pragma once
#include "Color.h"
#include "Object.h"
#include "core.h"
#include "Ray.h"
#include "Vec3.h"




namespace EPIC{


	enum material_type { LIGHT_MATERIAL=0, NOT_LIGHT_MATERIAL };

	class Material{
		public:
		virtual bool scatter(const Ray& in, const struct HitRecord& rec, Color& attenuation, Ray& out) const = 0;
		material_type type;
	};


	class Lambertian: public Material{
		
		public:

		Lambertian(const Color& c): albedo(c){
			type = NOT_LIGHT_MATERIAL;
		}

		virtual bool scatter(const Ray& in, const struct HitRecord& rec, Color& attenuation, Ray& out) const override{
			

			auto scatter_direction = *rec.normal + *Vec3<float>::random_unit_vector();
			// rec.normal points into the sphere, the scattered ray must leave it
			out = *std::make_shared<EPIC::Ray>(rec.position, *scatter_direction*-1.0f);
			attenuation = albedo;
			return true;
		}

		private:
		Color albedo;

	};


	class Metal: public Material{

		public: 
		Metal(const Color& c): albedo(c){
			type = NOT_LIGHT_MATERIAL;
		}

		virtual bool scatter(const Ray& in, const struct HitRecord& rec, Color& attenuation, Ray& out) const override{

			auto reflected = Vec3<float>::reflect(*(*in.direction()*in.norm), *rec.normal);
			out = *std::make_shared<EPIC::Ray>(rec.position, reflected);
			attenuation = albedo;

			auto cond = (*in.direction()*in.norm)->dot(*rec.normal);
			return cond > 0;
		}

		private:
		Color albedo;
	};


	class LightMat : public Material {

	public:
		LightMat(const Color& c) : albedo(c) {
			type = LIGHT_MATERIAL;
		}

		virtual bool scatter(const Ray& in, const struct HitRecord& rec, Color& attenuation, Ray& out) const override {
			attenuation = albedo;
			return true;
		}

	private:
		Color albedo;
	};

}
