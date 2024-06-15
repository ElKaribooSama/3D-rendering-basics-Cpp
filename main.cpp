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

Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}

float winding(Vec3f t0, Vec3f t1,Vec3f p) {
	return (p.x-t0.x)*(t1.y-t0.y) - (p.y-t0.y)*(t1.x-t0.x);
}

Vec3f cross(Vec3f v1, Vec3f v2) {
    return Vec3f(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];

	s[0].x = C.x-A.x;
    s[0].y = B.x-A.x;
    s[0].z = A.x-P.x;

	s[1].x = C.y-A.y;
    s[1].y = B.y-A.y;
    s[1].z = A.y-P.y;

    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u.z)>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color) {
	Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1, image.get_height()-1);
	
    for (int i=0; i<3; i++) {
        bboxmin.x = std::max(0.f,      std::min(bboxmin.x, pts[i].x));
		bboxmax.x = std::min(clamp.x,  std::max(bboxmax.x, pts[i].x));
		bboxmin.y = std::max(0.f,      std::min(bboxmin.y, pts[i].y));
		bboxmax.y = std::min(clamp.y,  std::max(bboxmax.y, pts[i].y));
    }
	
    Vec3f P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) { 
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) { 
            Vec3f bc_screen  = barycentric(pts[0],pts[1],pts[2], P); 
			P.z =  pts[0].z*bc_screen.x;
			P.z += pts[0].z*bc_screen.y;
			P.z += pts[0].z*bc_screen.z;

            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
			if (zbuffer[int(P.x+P.y*width)] < P.z) {
				zbuffer[int(P.x+P.y*width)] = P.z;
            	image.set(P.x, P.y, color);
			}
        } 
    } 
}

int main(int argc, char** argv) {
	if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }


	Vec3f light_dir(0,0,-1); // define light_dir

	float *zbuffer = new float[width*height];
    for (int i=width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    TGAImage image(width, height, TGAImage::RGB);

	TGAImage texture;
	texture.read_tga_file("obj/african_head_diffuse.tga");

    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);

        Vec3f pts[3];
        for (int i=0; i<3; i++) pts[i] = world2screen(model->vert(face[i]));
		Vec3f n = (model->vert(face[2])-model->vert(face[0]))^(model->vert(face[1])-model->vert(face[0]));  
		n.normalize(); 

		float intensity = n*light_dir; 
		if (intensity>0) {
        	triangle(pts, zbuffer, image, TGAColor(rand()%255,rand()%255,rand()%255,255));
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