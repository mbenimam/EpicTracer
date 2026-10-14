#pragma once
#include "Vec3.h"
#include "Ray.h"
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

namespace EPIC{

    class Camera{
        public:
        Camera(
            const Vec3<float>& lookfrom,
            const Vec3<float>& lookat,
            const Vec3<float>& vup,
            float vfov,
            float aspect_ratio,
            float aperture,
            float focus_dist,
            int image_width,
            int image_height);

        inline static std::shared_ptr<Vec3<float>> random_in_unit_disk(){

            auto p = std::make_shared<EPIC::Vec3<float>>((random_double()-0.5f)*2.0, (random_double()-0.5f)*2.0f, 0);
            do{
                p = std::make_shared<EPIC::Vec3<float>>((random_double()-0.5f)*2.0, (random_double()-0.5f)*2.0f, 0);

            } while(p->dot(*p) >= 1.0f);
            return p;
        }

        // Ray through a random point of pixel (row i, column j), from a random point of the lens
        std::shared_ptr<Ray> getRay(int i, int j) const;

        private:
        Vec3<float> origin;
        Vec3<float> lower_left_corner;
        Vec3<float> horizontal, vertical;

        std::shared_ptr<Vec3<float>> x, y, z;
        float lens_radius;
        int width, height;
    };

}
