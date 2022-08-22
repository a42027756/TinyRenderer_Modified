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
	Model(const char* filename); //���캯����Ҳ�ǽ���.obj�ļ������庯��
	~Model();
	int nverts(); //���ض�������
	int nfaces(); //��������������
	Vec3f vert(int i); //��������Ϊi�Ķ���xyz���ݣ�������Vec3f��
	Vec2f uv(int i); //��������Ϊi��uv���ݣ�������Vec2f��
	Vec3f normal(int i);
	std::vector<int> face(int idx); //��������Ϊidx�������������ݣ����涥�������i
};

#endif //__MODEL_H__