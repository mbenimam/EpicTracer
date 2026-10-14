#pragma once
#include "Vec3.h"
#include "Ray.h"
#include <vector>
#include <memory>



namespace EPIC{

	class Material;

	struct HitRecord{
		std::shared_ptr<Vec3<float>> position;
		std::shared_ptr<Vec3<float>> normal;
		std::shared_ptr<Material> m_material;
		float t;

		HitRecord():position(nullptr), normal(nullptr), m_material(nullptr){
			t = INFINITY;
		}
	};


	class Hitable{
		public:
		virtual bool hit(std::shared_ptr<Ray> Ray, float t_min, float t_max, HitRecord& record) const{ return false; };
	};


	class HitList: Hitable{

		public:
		void add(std::shared_ptr<Hitable> obj){
			objects.push_back(obj);
		}

		virtual bool hit(std::shared_ptr<Ray> ray, float t_min, float t_max, HitRecord& record) const {

			bool hit_something = false;
			auto closest_so_far = t_max;



			HitRecord temp_rec;

			for(auto object:objects){
				if(object->hit(ray, t_min, t_max, temp_rec)){
					if(closest_so_far>temp_rec.t){
						hit_something = true;
						closest_so_far = temp_rec.t;
						record = temp_rec;
						
					}
				}
			}

			return hit_something;
		}

		private:
		std::vector<std::shared_ptr<Hitable>> objects;
	};


	class Object{

		public:

		virtual std::shared_ptr<Vec3<float>> normalAt(std::shared_ptr<Vec3<float>> point) const{ return std::make_shared<Vec3<float>>(); };
		virtual const Vec3<float>& scatter(const Vec3<float>& point) const{ return point; };

		void move(const Vec3<float>& position){
			*m_position = position;
		}


		protected:
		std::shared_ptr<Vec3<float>> m_position;		
	};



}

