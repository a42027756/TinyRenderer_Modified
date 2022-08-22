#pragma once
#include "geometry.h"
#include "tgaimage.h"

namespace rst {
	void triangle_line_sweeping(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color);
	void triangle(Vec3f pts[3], float* zbuffer, TGAImage& image, TGAColor color, const int& width);
	Vec3f barycentric(Vec3f pts[3], Vec3f P);
	void triangle_texture(Vec3f pts[3], Vec2f uvs[3], Vec3f norms[3], float* zbuffer, TGAImage& image, TGAImage& texture, const int& width, float intensity);
	void triangle_texture(Vec3f pts[3], Vec2f uvs[3], Vec3f norms[3], float* zbuffer, TGAImage& image, TGAImage& texture, const int& width, Vec3f light_dir);
}