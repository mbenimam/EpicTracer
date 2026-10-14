#pragma once

#include "Vec3.h"
#include "Color.h"
#include "Ray.h"
#include "Object.h"

namespace EPIC{
	class Sphere: virtual public Object, public Hitable{

		public:
		Sphere(std::shared_ptr<Vec3<float>> origin, float radius, std::shared_ptr<Material> c):m_material(c){
			Object::m_position = origin;
			m_radius = radius;
		}


		virtual std::shared_ptr<Vec3<float>> normalAt(const Vec3<float>& point) const {
			return (*Object::m_position - point)->normalize();
		}


		virtual const Vec3<float>& scatter(const Vec3<float>& point) const {
			return point;
		}


		virtual bool hit(std::shared_ptr<Ray> ray, float t_min, float t_max, HitRecord& record) const{

			auto sphereToRay = (*ray->origin()) - (*m_position);
			float b = sphereToRay->dot(*ray->direction())*2.0;
			float c = sphereToRay->dot(*sphereToRay) - m_radius*m_radius;
			float delta = b*b-4*c;
			if(delta<0)
				return false;

			auto sqrt_d = sqrt(delta);
			auto root = (float)(-b-sqrt_d)/2.0f;

			if(root < t_min||root > t_max){
				root = (float)(-b+sqrt_d)/2.0f;
				if(root < t_min||root > t_max){
					return false;
				}
			}

			record.t = root;
			record.position = ray->pointAt(root);
			record.normal = normalAt(*record.position);
			record.m_material = m_material;

			return true;
		}

		float m_radius;
		protected:
		std::shared_ptr<Material> m_material;
	};
}
