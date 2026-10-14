#include "Camera.h"
#include "Ray.h"
#include <memory>


namespace EPIC{

        Camera::Camera(
            const Vec3<float>& lookfrom,
            const Vec3<float>& lookat,
            const Vec3<float>& vup,
            float vfov,
            float aspect_ratio,
            float aperture,
            float focus_dist,
            int image_width,
            int image_height): origin(lookfrom), width(image_width), height(image_height){

            auto theta = vfov*M_PI/180.0;
            auto h = tan(theta/2);
            auto viewport_height = h;
            auto viewport_width = aspect_ratio*viewport_height;

            z = (lookfrom-lookat)->normalize();
            x = vup.cross(*z)->normalize();
            y = z->cross(*x);

            // The viewport sits on the focus plane, so points on it stay sharp
            horizontal = *(*x*(focus_dist*viewport_width*2.0f));
            vertical = *(*y*(focus_dist*viewport_height*2.0f));

            lower_left_corner = *(*(*(origin-*(horizontal*0.5f))-*(vertical*0.5f))-*(*z*focus_dist));

            lens_radius = aperture/2;
        }

        std::shared_ptr<Ray> Camera::getRay(int i, int j) const{

            float i_n = (i+random_double())/height;
            float j_n = (j+random_double())/width;

            auto rd = *(*random_in_unit_disk()*lens_radius);
            auto offset = *(*(*x*rd[0])+*(*y*rd[1]));

            auto shoot_pos = origin+offset;

            auto viewport_pos = *(*(lower_left_corner+
                                  *(vertical*i_n))+
                                  *(horizontal*j_n));

            return std::make_shared<Ray>(shoot_pos, viewport_pos-*shoot_pos);

        }

}
