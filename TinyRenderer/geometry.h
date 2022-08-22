#pragma once

/**/


#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>
#include <vector>

#define PI 3.14159526


using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//构建Vec2<>类
template <class t> struct Vec2 {
	union {
		struct { t u, v; };
		struct { t x, y; };
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u), v(_v) {}
	inline Vec2<t> operator +(const Vec2<t>& V) const { return Vec2<t>(u + V.u, v + V.v); }
	inline Vec2<t> operator -(const Vec2<t>& V) const { return Vec2<t>(u - V.u, v - V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u * f, v * f); }
	template <class> friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

//构建Vec3类
template <class t> struct Vec3 {
	union {
		struct { t x, y, z; };
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	//Vec3<t>() : x(t()), y(t()), z(t()) {}
	Vec3() : x(0), y(0), z(0) {}
	Vec3<t>(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	template <class u> Vec3<t>(const Vec3<u>& v);
	//Vec3<t>(const Vec3<t> &v) : x(t()), y(t()), z(t()) {*this = v}
	inline Vec3<t> operator ^(const Vec3<t>& v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline Vec3<t> operator +(const Vec3<t>& v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<t> operator -(const Vec3<t>& v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<t> operator -()                 const { return Vec3<t>(-x, -y, -z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x * f, y * f, z * f); }
	inline t       operator *(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z; }
	inline t	   operator [](const int i)     const { 
		switch (i) {
		case 0:
			return x;
			break;
		case 1:
			return y;
			break;
		case 2:
			return z;
			break;
		default:
			std::cerr << "Not supposed tp be here!";
			return 0;
		}

	}
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	Vec3<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }
	Vec3<t> cross(Vec3<t>& v) {
		Vec3<t> temp;
		temp.x = y * v.z - z * v.y;
		temp.y = z * v.x - x * v.z;
		temp.z = x * v.y - y * v.x;
		return temp;
	}
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};


//typedef 别名
typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

//姑且理解为强制转换
template <> template <> Vec3<int>::Vec3(const Vec3<float>& v);
template <> template <> Vec3<float>::Vec3(const Vec3<int>& v);


//重载输出流<<
template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

/////////////////////////////////////////////////////////


const int DEFAULT_ALLOC = 4;

class Matrix {
	std::vector<vector<float>> m;
	int rows, cols;
public:
	Matrix(int r = DEFAULT_ALLOC, int c = DEFAULT_ALLOC);
	inline int nrows();
	inline int ncols();

	static Matrix identity(int dimensions);
	vector<float>& operator[](const int i);
	Matrix operator*(const Matrix& a);
	Matrix transpose();
	Matrix inverse();

	friend std::ostream& operator<< (std::ostream& s, Matrix& m);
};


namespace Utils {
	static float radians(float input) {
		return input * PI / 180.0;
	}
}


#endif //__GEOMETRY_H__