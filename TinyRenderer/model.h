#pragma once
#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
	std::vector<Vec2f> uvs_;
	std::vector<Vec3f> normals_;
public:
	Model(const char* filename); //构造函数，也是解析.obj文件的主体函数
	~Model();
	int nverts(); //返回顶点数量
	int nfaces(); //返回三角形面数
	Vec3f vert(int i); //返回索引为i的顶点xyz数据，储存在Vec3f中
	Vec2f uv(int i); //返回索引为i的uv数据，储存在Vec2f中
	Vec3f normal(int i);
	std::vector<int> face(int idx); //返回索引为idx的三角形面数据，储存顶点的索引i
};

#endif //__MODEL_H__