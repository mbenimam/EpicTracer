#include <iostream>
#include "Vec3.h"
#include "Image.h"
#include "Color.h"
#include "core.h"
#include "Ray.h"
#include "Sphere.h"
#include "Object.h"
#include "Camera.h"
#include "Material.h"
#include <iomanip>
#include <string>
#include <ostream>
#include <cstdlib>
#include <stdexcept>
#define _USE_MATH_DEFINES
#include <math.h>
#ifdef _OPENMP
#include <omp.h>
#endif


std::shared_ptr<EPIC::Color> rayTrace(std::shared_ptr<EPIC::Ray> ray, const EPIC::HitList& world, EPIC::Image& bg, int depth){

	if(depth<=0){
		return std::make_shared<EPIC::Color>("000000");
	}
	
	EPIC::HitRecord rec;
	if(world.hit(ray, 0.001f, 100000.0f, rec)){

		auto material = rec.m_material;
		auto reflectedRay = std::make_shared<EPIC::Ray>();
		auto attenuation = std::make_shared<EPIC::Color>();

		if(material->scatter(*ray, rec, *attenuation, *reflectedRay)){

			if (material->type == EPIC::LIGHT_MATERIAL) {
				return std::make_shared<EPIC::Color>(*(*attenuation*3.0f));
			}
			else {
				auto c = rayTrace(reflectedRay, world, bg, depth - 1);
				return std::make_shared<EPIC::Color>(*(*attenuation * *c));
			}
		}

		return std::make_shared<EPIC::Color>("000000");
	}

	// The ray left the scene: it sees the environment map
	return bg.sample(*ray->direction());
}


void usage(const char* program){
	std::cout<<"Usage: "<<program<<" [options]\n"
	         <<"  --width N    image width in pixels (default 1920)\n"
	         <<"  --height N   image height in pixels (default 1080)\n"
	         <<"  --spp N      samples per pixel (default 100)\n"
	         <<"  --depth N    maximum number of bounces (default 20)\n"
	         <<"  --sky FILE   environment map (default assets/background.hdr)\n"
	         <<"  --out FILE   output image, PPM format (default render.ppm)\n";
}


int main(int argc, char** argv){

	EPIC::RenderSettings settings;
	std::string sky = "assets/background.hdr";
	std::string out = "render.ppm";

	for(int a = 1; a<argc; a++){
		std::string option = argv[a];
		if(option=="--help"||option=="-h"){
			usage(argv[0]);
			return 0;
		}
		if(a+1>=argc){
			usage(argv[0]);
			return 1;
		}
		std::string value = argv[++a];
		if(option=="--width") settings.width = std::atoi(value.c_str());
		else if(option=="--height") settings.height = std::atoi(value.c_str());
		else if(option=="--spp") settings.samples_per_pixel = std::atoi(value.c_str());
		else if(option=="--depth") settings.max_depth = std::atoi(value.c_str());
		else if(option=="--sky") sky = value;
		else if(option=="--out") out = value;
		else{
			usage(argv[0]);
			return 1;
		}
	}

	if(settings.width<=0||settings.height<=0||settings.samples_per_pixel<=0||settings.max_depth<=0){
		usage(argv[0]);
		return 1;
	}

	const int WIDTH = settings.width;
	const int HEIGHT = settings.height;
	const int SAMPLES_PER_PIXEL = settings.samples_per_pixel;
	const int MAX_DEPTH = settings.max_depth;

	// background
	std::unique_ptr<EPIC::Image> bg;
	try{
		bg = std::make_unique<EPIC::Image>(sky.c_str());
	}
	catch(const std::runtime_error& e){
		std::cerr<<e.what()<<" (run from the repository root, or pass --sky)\n";
		return 1;
	}

	//camera
	auto p = EPIC::Vec3<float>(-1.9f, 1.2f, -3.0f);
	auto p2 = EPIC::Vec3<float>(-2.9f, 1.5f, 4.54f);

	EPIC::Camera camera(p,
						p2,
						EPIC::Vec3<float>(0.0f, 1.0f, 0.0f),
						45,
						(float)WIDTH/(float)HEIGHT,
						0.5f,
						(p-p2)->norm(),
						WIDTH,
						HEIGHT);

	// world
	EPIC::HitList world;

	auto color1 = std::make_shared<EPIC::Color>("212121"); // plan
	auto color2 = std::make_shared<EPIC::Color>("CCCCCC"); // silver
	auto color3 = std::make_shared<EPIC::Color>("536DFE"); // zerga
	auto color4 = std::make_shared<EPIC::Color>("E91E63"); // rose
	auto color5 = std::make_shared<EPIC::Color>("CDDC39"); // khedhra


	auto lambertian1 = std::make_shared<EPIC::Lambertian>(*color1);
	auto lambertian2 = std::make_shared<EPIC::Metal>(*color2);
	auto lambertian3 = std::make_shared<EPIC::Metal>(*color3);
	auto lambertian4 = std::make_shared<EPIC::Metal>(*color4);
	auto lambertian5 = std::make_shared<EPIC::Metal>(*color5);
	auto lightColor = std::make_shared<EPIC::LightMat>(*color2);


	world.add(std::make_shared<EPIC::Sphere>(std::make_shared<EPIC::Vec3<float>>(-5.58f, 2.5f, 9.38f), 2.5f, lambertian5));
	world.add(std::make_shared<EPIC::Sphere>(std::make_shared<EPIC::Vec3<float>>(3.16f, 2.0f, 5.5f), 1.0f, lambertian4));
	world.add(std::make_shared<EPIC::Sphere>(std::make_shared<EPIC::Vec3<float>>(-2.9f, 1.5f, 5.54f), 1.5f, lambertian3));
	world.add(std::make_shared<EPIC::Sphere>(std::make_shared<EPIC::Vec3<float>>(-1.5f, 1.0f, 3.6f), 1.0f, lambertian2));

	// flooor
	world.add(std::make_shared<EPIC::Sphere>(std::make_shared<EPIC::Vec3<float>>(0.0f, -300.0f, 1.0f), 300.0f, lambertian1));

	EPIC::Image img(WIDTH, HEIGHT);

	auto total_steps = (size_t)HEIGHT*WIDTH;

	size_t steps_completed = 0;


	#pragma omp parallel
	{
		#ifdef _OPENMP
		seed_random(omp_get_thread_num()+1);
		#endif

		#pragma omp for
		for(int i = 0; i<HEIGHT; i++){
			for(int j = 0; j<WIDTH; j++){
				auto c = std::make_shared<EPIC::Vec3<float>>();
				for(int sample = 0; sample<SAMPLES_PER_PIXEL; sample++){
					auto ray = camera.getRay(i, j);
					std::shared_ptr<EPIC::Vec3<float>> conversion = rayTrace(ray, world, *bg, MAX_DEPTH);
					c = *c + *conversion;
				}

				// A pixel is the average of its samples
				img.getPixel(HEIGHT-i-1, WIDTH-j-1) = std::make_shared<EPIC::Color>(*(*c/(float)SAMPLES_PER_PIXEL));

				size_t done;
				#pragma omp atomic capture
				done = ++steps_completed;

				if(done%10000==1){
					#pragma omp critical
					std::cout<<"Progress: "<<done<<" of "<<total_steps<<" ("<< std::fixed << std::setprecision(1)<<(100.0*done/total_steps)<<"%)\n";
				}
			}
		}
	}

	img.save(out.c_str());

	return 0;
}
