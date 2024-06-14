#include "tgaimage.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
	size_t length = std::max(std::abs(y1 - y0),std::abs(x1 - x0));
	float step = 1. / length;

	for (float t = 0; t < 1; t+= step) {
		int x = x0 + (x1-x0)*t;
		int y = y0 + (y1-y0)*t;
		image.set(x,y,color);
	}
}

int main(int argc, char** argv) {
	TGAImage image(100, 100, TGAImage::RGB);
	image.set(52, 41, red);
	line(20, 13, 40, 80, image, red); 
	line(80, 40, 13, 20, image, red);
	line(13, 20, 80, 40, image, white); 
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}