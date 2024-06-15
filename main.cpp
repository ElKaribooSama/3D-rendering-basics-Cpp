#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const TGAColor blue  = TGAColor(0,   0,   255, 255);
const TGAColor yellow= TGAColor(255, 255,   0, 255);
const TGAColor purple= TGAColor(255,   0, 255, 255);

Model *model = NULL;
const int width  = 800;
const int height = 800;

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) {
	size_t length = std::max(std::abs(t1.y - t0.y),std::abs(t1.x - t0.x));
	float step = 1. / (length + 1);

	for (float t = 0; t < 1; t+= step) {
		int x = t0.x + (t1.x-t0.x)*t;
		int y = t0.y + (t1.y-t0.y)*t;
		image.set(x,y,color);
	}
}

float winding(Vec2f t0, Vec2f t1,Vec2f p) {
	return (p.x-t0.x)*(t1.y-t0.y) - (p.y-t0.y)*(t1.x-t0.x);
}

void triangle_boundingbox(Vec2f t0, Vec2f t1, Vec2f t2, TGAImage &image, TGAColor color) {
	Vec2f b0,b1;
	b0 = Vec2f(std::min(std::min(t0.x,t1.x),t2.x),std::min(std::min(t0.y,t1.y),t2.y));
	b1 = Vec2f(std::max(std::max(t0.x,t1.x),t2.x),std::max(std::max(t0.y,t1.y),t2.y));

	for (int x = b0.x; x < b1.x; x++) {
		for (int y = b0.y; y < b1.y; y++) {
			if (winding(t2,t0,Vec2f(x,y)) < 0 && winding(t1,t2,Vec2f(x,y)) < 0 && winding(t0,t1,Vec2f(x,y)) < 0) {
				image.set(x,y,color);
			}
		}
	}
}

void triangle_linesweep(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
	if (t0.y>t1.y) std::swap(t0, t1); 
    if (t0.y>t2.y) std::swap(t0, t2); 
    if (t1.y>t2.y) std::swap(t1, t2);
    line(t0, t1, image, blue); 
    line(t1, t2, image, green); 
    line(t2, t0, image, red); 

	float t0s = (float)(t0.x - t2.x) / (t0.y - t2.y);
	float t1s = (float)(t0.x - t1.x) / (t0.y - t1.y);
	float t2s = (float)(t1.x - t2.x) / (t1.y - t2.y);
	
	for (int y = t0.y; y < t1.y; y += 1) {
		line(Vec2i(t0.x + (y - t0.y) * t0s,y)
			,Vec2i(t0.x + (y - t0.y) * t1s,y),image,color);
	}

	for (int y = t1.y; y < t2.y; y += 1) {
		line(Vec2i(t0.x + (y - t0.y) * t0s,y)
			,Vec2i(t1.x + (y - t1.y) * t2s,y),image,color);
	}
}

int main(int argc, char** argv) {
	if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);

	Vec3f light_dir(0,0,-1); // define light_dir

	for (int i=0; i<model->nfaces(); i++) { 
		std::vector<int> face = model->face(i); 
		Vec2f screen_coords[3]; 
		Vec3f world_coords[3]; 
		for (int j=0; j<3; j++) { 
			Vec3f v = model->vert(face[j]); 
			screen_coords[j] = Vec2f((v.x+1.)*width/2., (v.y+1.)*height/2.); 
			world_coords[j]  = v; 
		} 
		Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
		n.normalize(); 
		float intensity = n*light_dir; 
		if (intensity>0) { 
			triangle_boundingbox(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255)); 
		} 
	}

	// Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
	// Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
	// Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
	// triangle_boundingbox(t0[0], t0[1], t0[2], image, red); 
	// triangle_boundingbox(t1[0], t1[1], t1[2], image, white); 
	// triangle_boundingbox(t2[0], t2[1], t2[2], image, green);

	

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}