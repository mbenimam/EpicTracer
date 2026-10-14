#pragma once
#include <iostream>
#include <random>
#include "core.h"
#include <math.h>
#include <memory>

namespace EPIC{

	template <typename T> class Vec3{
		public:

		Vec3() : m_data{ 0.0f, 0.0f, 0.0f }{};
		Vec3(T x, T y, T z) : m_data{x, y, z}{};
		Vec3(const Vec3& v) : m_data{v[0], v[1], v[2]}{};

		inline std::shared_ptr<Vec3<T>> operator+(const Vec3<T>& v) const{
			return std::make_shared<Vec3<T>>(m_data[0]+v[0],
									    m_data[1]+v[1],
									    m_data[2]+v[2]);
		};
		inline std::shared_ptr<Vec3<T>> operator-(const Vec3<T>& v) const{
			return std::make_shared<Vec3<T>>(m_data[0]-v[0], // empty
											 m_data[1]-v[1],
											 m_data[2]-v[2]);
		};
		inline std::shared_ptr<Vec3<T>> operator*(const float& k) const{
			return std::make_shared<Vec3<T>>(m_data[0]*k, // empty
											 m_data[1]*k,
											 m_data[2]*k);
		};

		inline std::shared_ptr<Vec3<T>> operator*(const Vec3& v) const{
			return std::make_shared<Vec3<T>>(m_data[0]*v[0], // empty
											 m_data[1]*v[1],
											 m_data[2]*v[2]);
		};

		inline std::shared_ptr<Vec3<T>> operator/(const float& k) const{
			return std::make_shared<Vec3<T>>(m_data[0]/k, // empty
											 m_data[1]/k,
											 m_data[2]/k);
		};



		inline static std::shared_ptr<Vec3<T>> random(){
			return std::make_unique<Vec3<T>>(random_double(), random_double(), random_double());
		}

		inline static std::shared_ptr<Vec3<T>> random(float min, float max){
			return std::make_shared<Vec3<T>>(random_double(min, max),
											 random_double(min, max),
											 random_double(min, max));
		}

		inline static std::shared_ptr<Vec3<T>> random_unit_sphere(){
			while(true){
				auto p = random(-1.0f, 1.0f);
				if(p->norm()>=1) continue;
				return p;
			}
		}

		inline static std::shared_ptr<Vec3<T>> random_unit_vector(){
			return random_unit_sphere()->normalize();
		}

		T& operator[](int index){
			return m_data[index];
		};
		T operator[](int index) const{
			return m_data[index];
		};

		inline std::shared_ptr<Vec3<T>> cross(const Vec3<T>& v) const{
			return std::make_shared<Vec3<T>>(m_data[1]*v[2]-m_data[2]*v[1],
											 m_data[2]*v[0]-m_data[0]*v[2],
											 m_data[0]*v[1]-m_data[1]*v[0]);
		};

		inline float norm()  const{
			return sqrt((m_data[0])*(m_data[0])+
						(m_data[1])*(m_data[1])+
						(m_data[2])*(m_data[2]));
		};
		inline std::shared_ptr<Vec3<T>> normalize(){
			return std::make_shared<Vec3<T>>(m_data[0]/this->norm(),
											 m_data[1]/this->norm(),
											 m_data[2]/this->norm());
		};

		inline float dot(const Vec3<T>& v) const{
			return m_data[0]*v[0]+
				m_data[1]*v[1]+
				m_data[2]*v[2];
		};
		 
		inline static std::shared_ptr<Vec3<T>> reflect(const Vec3<T>& in, const Vec3<T>& normal){
			return in - *(normal*(in.dot(normal)*2.0f));
		}

		protected:
		T m_data[3];


	};



	template<typename T>
	std::ostream& operator<< (std::ostream& out, const Vec3<T>& v){
		out<<v[0]<<" "<<v[1]<<" "<<v[2]<<std::endl;
		return out;
	}

}
