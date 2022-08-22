// TinyRenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "tgaimage.h"
#include <cmath>
#include "model.h"
#include "geometry.h"
#include "Rasterization.h"
#include <limits.h>
#include <math.h>


using namespace std;

bool steep = false;

const static int width = 800;
const static int height = 600;
const int depth = 255;


//矩阵转向量
Vec3f m2v(Matrix m) {
    //return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
    return Vec3f(m[0][0], m[1][0], m[2][0]);  //分开透视除法
}

//向量转矩阵
Matrix v2m(Vec3f v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

//TinyRenderer视口矩阵
//(x, y)：视口起始点
//(w, h)：视口长宽
//和z有关的操作是为了将z从(-1, 1)转成(0, depth)
Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x + w / 2.f;
    m[1][3] = y + h / 2.f;
    m[2][3] = depth / 2.f;

    m[0][0] = w / 2.f;
    m[1][1] = h / 2.f;
    m[2][2] = depth / 2.f;
    return m;
}

Matrix Viewport(int width, int height) {
    Matrix m = Matrix::identity(4);
    m[0][0] = width * 0.5;
    m[1][1] = height * 0.5;
    m[0][3] = width * 0.5;
    m[1][3] = height * 0.5;
    return m;
}

Matrix LookAt(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f forward = (eye - center).normalize();
    Vec3f right = up.cross(forward).normalize();
    Vec3f cameraUp = forward.cross(right).normalize();
    Matrix res1 = Matrix::identity(4);
    for (int i = 0; i < 3; ++i) {
        res1[0][i] = right[i];
        res1[1][i] = cameraUp[i];
        res1[2][i] = forward[i];
        //res[i][3] = -center[i];
    }
    Matrix res2 = Matrix::identity(4);
    for (int j = 0; j < 3; ++j) {
        res2[j][3] = -eye[j];
    }
    return res1 * res2;
}

Matrix Perspective(float FOV, float aspect_ratio, float near, float far) {
    Matrix m = Matrix::identity(4);
    float fov = Utils::radians(FOV);
    m[0][0] = 1 / (aspect_ratio * tan(fov * 0.5));
    m[1][1] = 1 / tan(fov * 0.5);
    m[2][2] = -(near + far) / far - near;
    m[2][3] = -2 * far * near / far - near;
    m[3][2] = -1;
    m[3][3] = 0;
    return m;
}

bool PerspectiveDivision(Matrix& m) {
    m[0][0] /= m[3][0];
    m[1][0] /= m[3][0];
    m[2][0] /= m[3][0];
    for (int i = 0; i < 3; i++) {
        if (m[i][0] < -m[3][0] || m[i][0] > m[3][0]) {
            return false;
        }

    }
    m[3][0] = 1.f;
    return true;
}

Vec3f light_dir(0, 0, -1);
Vec3f camera(0, 1, 3);


Matrix Projection = Perspective(90.0, 1.333, 0.01, 100);
Matrix Lookat = LookAt(camera, Vec3f(0, 0, 1), Vec3f(0, 1, 0));
Matrix ViewportMat = Viewport(width, height);
//Matrix Viewport = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);


//const Vec3f light_dir = Vec3f(0, 0, -1);


//最简单的直线算法 t可以理解成采样个数 如果循环次数最终小于max(x1 - x0, y1 - y0)，线段在视觉上就不会是连续的
void line1(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    //某种程度上是一个lerp，只是将lerp写成了一个for循环（因为t是0到1）
    for (float t = 0.; t < 1.; t += .016) {
        int x = x0 + (x1 - x0) * t;
        int y = y0 + (y1 - y0) * t;
        image.set(x, y, color);
    }
}

//直线算法2 相比算法1更为直观
//依然是插值的思想，参数t落在[0, 1]，然后插值出y当前的值
void line2(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    for (int x = x0; x <= x1; x++) {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        if (steep) {
            image.set(y, x, color);
        } else
            image.set(x, y, color);
    }
}

//直线算法2 但是是以y为循环最大值
void line2Transpose(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    for (int x = x0; x <= x1; x++) {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        image.set(y, x, color);
    }
}

//float funcLine(int x0, int y0, int x1, int y1, int x) {
//    return (x - x0 / (float)(x1 - x0)) * (float)(y1 - y0) + y0;
//}

//void lineBresenham(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
//    int dy = y1 - y0;
//    int dx = x1 - x0;
//
//    for (int x = x0; x <= x1; x++) {
//        float y = funcLine(x0, y0, x1, y1, x);
//        if ()
//    }
//}

//float version of bresenham
void line4(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    float derror = abs(dy / (float)dx);
    float error = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);
        }
        else
            image.set(x, y, color);
        error += derror;
        if (error > .5) {
            y += (y1 > y0 ? 1 : -1);
            error -= 1;
            //error = 0;
        }
    }
}


//standard bresenham algorithm
void line5(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    //optimization for line3() : add abs()
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int D = 2 * dy - dx;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);

        }
        else {
            image.set(x, y, color);
        }
        if (D > 0) {
            if (y1 > y0) {
                y++;
            }
            else {
                y--;
            }
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

void line3(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    //判断xy之间大小 记得要加绝对值
    if (abs(x1 - x0) >= abs(y1 - y0)) {
        //判断x之间大小
        if (x1 >= x0) {
            steep = false;
            line5(x0, y0, x1, y1, image, color);
        }
        else {
            steep = false;
            line5(x1, y1, x0, y0, image, color);
        }

    }
    else {
        //判断y之间大小
        if (y1 >= y0) {
            steep = true;
            line5(y0, x0, y1, x1, image, color);
        }
        else {
            steep = true;
            line5(y1, x1, y0, x0, image, color);
        }
    }
}

//画非洲老哥的头
void drawAfrican(Model* model, TGAImage& image, int width, int height) {
    for (int i = 0; i < model->nfaces(); i++) {
        //设置face = face(i)，即第i个面的索引
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]); //获取第face[j]，即该索引的顶点坐标
            //获取第face[j + 1]的顶点坐标，之所以要%3是考虑到边界问题，也就是j = 2时，j + 1 = 3会产生数组越界，
            //用%3使其回到0来避免这种情况，同时也达到首尾相接的效果，组成三角形
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            //所有的* width / 2和 * height / 2 都是视口变换（Viewport Transformation）
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            line3(x0, y0, x1, y1, image, white);
        }
    }
}

//设置平行光源进行简单的Flat shading & back face culling
void simpleShading(Model* model, int width, int height, Vec3f light_dir, TGAImage& image, float * zbuffer) {
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        //screen_coords : 更新为Vec3f类，保存深度信息
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j * 3]);
            screen_coords[j] = Vec3f((v.x + 1.) * width / 2., (v.y + 1.) * height / 2., v.z);
            world_coords[j] = v;
        }
        //获取平面法向量
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        //计算法向量和光照的点乘
        float intensity = n * light_dir;
        //如果Intensity < 0，说明面片处于背面（摄像机看不到的位置），直接discard（不做渲染）
        if (intensity > 0) {
            rst::triangle(screen_coords, zbuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255), width);
        }
        //用Z-buffer做剔除(Hidden face removal)
        //rst::triangle(screen_coords, zbuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255), width);
    }
}

void textureShading(Model* model, int width, int height, TGAImage& image, TGAImage& texture, float* zbuffer) {
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        Vec2f uv_coords[3];
        Vec3f normal_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j * 3]);
            Vec2f tex = model->uv(face[j * 3 + 1]);
            Vec3f norm = model->normal(face[j * 3 + 2]);
            //screen_coords[j] = Vec3f((v.x + 1.) * width / 2., (v.y + 1.) * height / 2., v.z);
            //screen_coords[j] = m2v(Viewport * Projection * v2m(v));  //透视投影
            Matrix temp = Projection * Lookat * v2m(v);
            //cout << endl << temp << endl;
            if (PerspectiveDivision(temp)) {
                screen_coords[j] = m2v(ViewportMat * temp);
                world_coords[j] = v;
                uv_coords[j] = tex;
                normal_coords[j] = norm;
            }
            else {
                break;
            }
            
            
        }

        //获取平面法向量
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        //计算法向量和光照的点乘
        //float intensity = n * light_dir;
        float intensity = n * light_dir;
        //如果Intensity < 0，说明面片处于背面（摄像机看不到的位置），直接discard（不做渲染）
        if (intensity > 0) {
            rst::triangle_texture(screen_coords, uv_coords, normal_coords, zbuffer, image, texture, width, intensity);
        }
        
        //插值法向量
        //rst::triangle_texture(screen_coords, uv_coords, normal_coords, zbuffer, image, texture, width, light_dir);

        
    }
}



int main(int argc, char** argv) {
   /* int width = 200;
    int height = 200;*/
    

    TGAImage image(width, height, TGAImage::RGB);
    TGAImage texture;
    texture.read_tga_file("obj/african_head_diffuse.tga");
    texture.flip_vertically();
    //image.set(52, 41, red);
    //line1(13, 20, 80, 40, image, white);

    //测试Bresenham算法用例
   /* line3(13, 20, 80, 40, image, white);
    line3(20, 13, 40, 80, image, red);
    line3(80, 40, 13, 20, image, red);
    line3(20, 80, 40, 40, image, red);*/

    //加载african.obj
    Model* model = new Model("obj/african.obj");
    //cout << "uv: " + model->uv(2).x + " " + model->uv(2).y;


    //Wireframe模式绘制 African_head
    //drawAfrican(model, image, width, height);

    //line-sweeping 光栅化算法

    //Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
    //Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
    //Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
    ///*rst::triangle_line_sweeping(t0[0], t0[1], t0[2], image, red);
    //rst::triangle_line_sweeping(t1[0], t1[1], t1[2], image, white);
    //rst::triangle_line_sweeping(t2[0], t2[1], t2[2], image, green);*/
    //rst::triangle(t0, image, red);
    //rst::triangle(t1, image, white);
    //rst::triangle(t2, image, green);


    //bounding box光栅化算法
    
    //Vec2i pts[3] = { Vec2i(10,10), Vec2i(190, 160), Vec2i(100, 30) };
    //rst::triangle(pts, image, red);

    //随机色彩的flat shading
  /*  for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f world_coords = model->vert(face[j]);
            screen_coords[j] = Vec2i((world_coords.x + 1.) * width / 2., (world_coords.y + 1.) * height / 2.);
        }
        rst::triangle(screen_coords, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
    }*/

    //Temporary Z-buffer
    float* zbuffer = new float[width * height];
    for (int i = width * height; i--;) {
        zbuffer[i] = -std::numeric_limits<float>::max(); //初始化z-buffer，全部设为最大值
        //zbuffer[i] = 10000.0;
    }

    //cout << zbuffer[40000];
    


    
    //Projection[3][2] = -1.f / camera.z;

   /* Vec3f test(1, 2, 1);
    Matrix lookat = LookAt(camera, Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    Matrix project = Perspective(90.0, 1.333, 0.1, 100);
    Matrix Test = v2m(test);
    Test = project * lookat * Test;*/



    //simpleShading(model, width, height, light_dir, image, zbuffer);
    textureShading(model, width, height, image, texture, zbuffer);
    

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("Projection1.tga");
    delete model;
    delete[] zbuffer;
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
