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
const int width  = 200;
const int height = 200;

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) {
	size_t length = std::max(std::abs(t1.y - t0.y),std::abs(t1.x - t0.x));
	float step = 1. / (length + 1);

	for (float t = 0; t < 1; t+= step) {
		int x = t0.x + (t1.x-t0.x)*t;
		int y = t0.y + (t1.y-t0.y)*t;
		image.set(x,y,color);
	}
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
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
    TGAImage image(width, height, TGAImage::RGB);

	Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
	Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
	Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
	triangle(t0[0], t0[1], t0[2], image, red); 
	triangle(t1[0], t1[1], t1[2], image, white); 
	triangle(t2[0], t2[1], t2[2], image, green);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}