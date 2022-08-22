//#pragma once
//#include <cstdint>
//#include <fstream>
//#include <vector>
//
//#pragma pack(push,1)
//struct TGAHeader {
//    std::uint8_t  idlength{};
//    std::uint8_t  colormaptype{};
//    std::uint8_t  datatypecode{};
//    std::uint16_t colormaporigin{};
//    std::uint16_t colormaplength{};
//    std::uint8_t  colormapdepth{};
//    std::uint16_t x_origin{};
//    std::uint16_t y_origin{};
//    std::uint16_t width{};
//    std::uint16_t height{};
//    std::uint8_t  bitsperpixel{};
//    std::uint8_t  imagedescriptor{};
//};
//#pragma pack(pop)
//
//struct TGAColor {
//    std::uint8_t bgra[4] = {0,0,0,0};
//    std::uint8_t bytespp = {0};
//
//    TGAColor() = default;
//    TGAColor(const std::uint8_t R, const std::uint8_t G, const std::uint8_t B, const std::uint8_t A=255) : bgra{B,G,R,A}, bytespp(4) { }
//    TGAColor(const std::uint8_t *p, const std::uint8_t bpp) : bytespp(bpp) {
//        for (int i=bpp; i--; bgra[i] = p[i]);
//    }
//    std::uint8_t& operator[](const int i) { return bgra[i]; }
//};
//
//struct TGAImage {
//    enum Format { GRAYSCALE=1, RGB=3, RGBA=4 };
//
//    TGAImage() = default;
//    TGAImage(const int w, const int h, const int bpp);
//    bool  read_tga_file(const std::string filename);
//    bool write_tga_file(const std::string filename, const bool vflip=true, const bool rle=true) const;
//    void flip_horizontally();
//    void flip_vertically();
//    TGAColor get(const int x, const int y) const;
//    void set(const int x, const int y, const TGAColor &c);
//    int width()  const;
//    int height() const;
//private:
//    bool   load_rle_data(std::ifstream &in);
//    bool unload_rle_data(std::ofstream &out) const;
//
//    int w   = 0;
//    int h   = 0;
//    int bpp = 0;
//    std::vector<std::uint8_t> data = {};
//};
//
//
//const static TGAColor white = TGAColor(255, 255, 255, 255);
//const static TGAColor red = TGAColor(255, 0, 0, 255);
//const static TGAColor green = TGAColor(0, 255, 0, 255);
//


#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#pragma pack(push,1)
struct TGA_Header {
	char idlength;
	char colormaptype;
	char datatypecode;
	short colormaporigin;
	short colormaplength;
	char colormapdepth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char  bitsperpixel;
	char  imagedescriptor;
};
#pragma pack(pop)



struct TGAColor {
	union {
		struct {
			unsigned char b, g, r, a;
		};
		unsigned char raw[4];
		unsigned int val;
	};
	int bytespp;

	TGAColor() : val(0), bytespp(1) {
	}

	TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255) : b(B), g(G), r(R), a(A), bytespp(4) {
	}

	TGAColor(int v, int bpp) : val(v), bytespp(bpp) {
	}

	TGAColor(const TGAColor& c) : val(c.val), bytespp(c.bytespp) {
	}

	TGAColor(const unsigned char* p, int bpp) : val(0), bytespp(bpp) {
		for (int i = 0; i < bpp; i++) {
			raw[i] = p[i];
		}
	}

	TGAColor& operator =(const TGAColor& c) {
		if (this != &c) {
			bytespp = c.bytespp;
			val = c.val;
		}
		return *this;
	}
};


class TGAImage {
protected:
	unsigned char* data;
	int width;
	int height;
	int bytespp;

	bool   load_rle_data(std::ifstream& in);
	bool unload_rle_data(std::ofstream& out);
public:
	enum Format {
		GRAYSCALE = 1, RGB = 3, RGBA = 4
	};

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage& img);
	bool read_tga_file(const char* filename);
	bool write_tga_file(const char* filename, bool rle = true);
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	TGAColor get(int x, int y);
	bool set(int x, int y, TGAColor c);
	~TGAImage();
	TGAImage& operator =(const TGAImage& img);
	int get_width();
	int get_height();
	int get_bytespp();
	unsigned char* buffer();
	void clear();
};

const static TGAColor white = TGAColor(255, 255, 255, 255);
const static TGAColor red = TGAColor(255, 0, 0, 255);
const static TGAColor green = TGAColor(0, 255, 0, 255);

#endif //__IMAGE_H__
