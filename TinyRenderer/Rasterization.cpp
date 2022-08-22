#include <iostream>
#include "geometry.h"
#include "tgaimage.h"
#include "Rasterization.h"
#include <cmath>
#include <math.h>

using namespace std;


    //old-school: line-sweeping algorithm
    //alpha边不需要分段，只需要按着beta边绘制的同时绘制一次即可
    void rst::triangle_line_sweeping(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
        // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
        if (t0.y > t1.y) std::swap(t0, t1);
        if (t0.y > t2.y) std::swap(t0, t2);
        if (t1.y > t2.y) std::swap(t1, t2);
        int total_height = t2.y - t0.y;
        //绘制下半部分
        for (int y = t0.y; y <= t1.y; y++) {
            int segment_height = t1.y - t0.y + 1;
            float alpha = (float)(y - t0.y) / total_height;
            float beta = (float)(y - t0.y) / segment_height; // be careful with divisions by zero 
            Vec2i A = t0 + (t2 - t0) * alpha;
            Vec2i B = t0 + (t1 - t0) * beta;
            image.set(A.x, y, red);
            image.set(B.x, y, green);
            if (A.x > B.x) std::swap(A, B);
            //绘制横向的直线，填满三角形内部
            for (int j = A.x; j <= B.x; j++) {
                image.set(j, y, color); // attention, due to int casts t0.y+i != A.y 
            }
        }
        //绘制上半部分
        for (int y = t1.y; y <= t2.y; y++) {
            int segment_height = t2.y - t1.y + 1;
            float alpha = (float)(y - t0.y) / total_height;
            float beta = (float)(y - t1.y) / segment_height; // be careful with divisions by zero 
            Vec2i A = t0 + (t2 - t0) * alpha;
            Vec2i B = t1 + (t2 - t1) * beta;
            if (A.x > B.x) std::swap(A, B);
            //绘制横向的直线，填满三角形内部
            for (int j = A.x; j <= B.x; j++) {
                image.set(j, y, color); // attention, due to int casts t0.y+i != A.y 
            }
        }
    }

    //求解重心坐标
    //输入：数组pts[3]，指向一个顺序为点A、B、C的Vec2i数组
    // 
    //     P点为所求的重心坐标对应的点
    Vec3f rst::barycentric(Vec3f pts[3], Vec3f P) {
        int Xa = pts[0].x;
        int Xb = pts[1].x;
        int Xc = pts[2].x;
        int Ya = pts[0].y;
        int Yb = pts[1].y;
        int Yc = pts[2].y;
        float u1 = (float)Xa * Yb - Xb * Ya;
        float u = ((Ya - Yb) * P.x + (Xb - Xa) * P.y + u1) / ((Ya - Yb) * Xc + (Xb - Xa) * Yc + u1);
        float v1 = (float)Xa * Yc - Xc * Ya;
        float v = ((Ya - Yc) * P.x + (Xc - Xa) * P.y + v1) / ((Ya - Yc) * Xb + (Xc - Xa) * Yb + v1);
        float a = 1 - u - v;
        //return Vec3f(1 - u - v, u, v);

        //The order matters！！！！！
        return Vec3f(1 - u - v, v, u);
    }



    //标准的三角形光栅化算法
    void rst::triangle(Vec3f pts[3], float * zbuffer, TGAImage& image, TGAColor color, const int &width) {
        //先求出bounding box 偷懒了直接用两个min\max嵌套
       /* for (int i = 0; i < 2; ++i) {
            if (pts[i].x > pts[i + 1].x) {
                swap(pts[i].x, pts[i + 1].x)
            }
        }*/
        int minx = min(pts[0].x, min(pts[1].x, pts[2].x));
        int maxx = max(pts[0].x, max(pts[1].x, pts[2].x));
        int miny = min(pts[0].y, min(pts[1].y, pts[2].y));
        int maxy = max(pts[0].y, max(pts[1].y, pts[2].y));
        //两个for循环嵌套 这就是大规模并行计算的暴力解法？
        for (int i = minx; i <= maxx; ++i) {
            for (int j = miny; j <= maxy; ++j) {
                Vec3f P(i, j, 0);
                Vec3f coord = barycentric(pts, P);
                //optimization for small black holes （REALLY DISGUSTING）
                if (coord.x < -.01 || coord.y < -.01 || coord.z < -.01) continue;
                //利用重心坐标插值z
                for (int i = 0; i < 3; i++) {
                    P.z += pts[i].z * coord[i];
                }
                //z-buffering
                if (zbuffer[int(P.x + P.y * width)] < P.z) {
                    zbuffer[int(P.x + P.y * width)] = P.z;
                    image.set(P.x, P.y, color);
                }
            }
        }
    }

    void rst::triangle_texture(Vec3f pts[3], Vec2f uvs[3], Vec3f norms[3], float* zbuffer, TGAImage& image,
        TGAImage& texture, const int& width, Vec3f light_dir) {
        //先求出bounding box
        int minx = min(pts[0].x, min(pts[1].x, pts[2].x));
        int maxx = max(pts[0].x, max(pts[1].x, pts[2].x));
        int miny = min(pts[0].y, min(pts[1].y, pts[2].y));
        int maxy = max(pts[0].y, max(pts[1].y, pts[2].y));

        
        
         for (int i = minx; i <= maxx; ++i) {
            for (int j = miny; j <= maxy; ++j) {
                Vec3f P(i, j, 0);
                Vec2f T(0, 0);
                Vec3f N(0, 0, 0);
                Vec3f coord = barycentric(pts, P);
                int w = texture.get_width();
                int h = texture.get_height();

                //TGAColor color1 = texture.get(uvs)

                //optimization for small black holes （REALLY DISGUSTING）
                if (coord.x < -.01 || coord.y < -.01 || coord.z < -.01) continue;
                //利用重心坐标插值z和uv
                for (int i = 0; i < 3; i++) {
                    P.z += pts[i].z * coord[i];
                    T.x += uvs[i].x * coord[i];
                    //cout << "uv.x : " << T.x << endl;
                    T.y += uvs[i].y * coord[i];
                    //cout << "uv.y : " << T.y << endl;
                    N.x += norms[i].x * coord[i];
                    N.y += norms[i].y * coord[i];
                    N.z += norms[i].z * coord[i];
                    

                }

                //Texture Sampling
                //w *= T.x;
                //w = (int)(w * T.x);
                ////cout << "width of the texture: " << w << endl;
                //h = (int)(h * T.y);
                ////h *= T.y;
                ////cout << "height of the texture: " << h << endl;
                //TGAColor color = texture.get(w, h);

                TGAColor color = texture.get(uvs[0].x, uvs[0].y);
                
                //Normal Mapping & Lighting
                N.normalize();
                float intensity = N * -light_dir;
                

                if (intensity > 0) {
                    //z-buffering
                    if (zbuffer[int(P.x + P.y * width)] < P.z) {
                        zbuffer[int(P.x + P.y * width)] = P.z;
                        //image.set(P.x, P.y, TGAColor(color.r * intensity, color.g * intensity, color.b * intensity));
                        image.set(P.x, P.y, TGAColor(color.r * intensity, color.g * intensity, color.b * intensity));
                    }

                }

                ////z-buffering
                //    if (zbuffer[int(P.x + P.y * width)] < P.z) {
                //        zbuffer[int(P.x + P.y * width)] = P.z;
                //        image.set(P.x, P.y, TGAColor(color.r, color.g, color.b));
                //    }
               

            }
        }
    }

    void rst::triangle_texture(Vec3f pts[3], Vec2f uvs[3], Vec3f norms[3], float* zbuffer,
        TGAImage& image, TGAImage& texture, const int& width, float intensity) {
        //cout << "Triangle rasterization\n";
        //先求出bounding box
        int minx = min(pts[0].x, min(pts[1].x, pts[2].x));
        int maxx = max(pts[0].x, max(pts[1].x, pts[2].x));
        int miny = min(pts[0].y, min(pts[1].y, pts[2].y));
        int maxy = max(pts[0].y, max(pts[1].y, pts[2].y));



        for (int i = minx; i <= maxx; ++i) {
            for (int j = miny; j <= maxy; ++j) {
                Vec3f P(i, j, 0);
                Vec2f T(0, 0);
                Vec3f N(0, 0, 0);
                Vec3f coord = barycentric(pts, P);
                int w = texture.get_width();
                int h = texture.get_height();
                //optimization for small black holes （REALLY DISGUSTING）
                if (coord.x < -.01 || coord.y < -.01 || coord.z < -.01) continue;
                Vec3i colors[3] = {
                    Vec3i(255,0,0),
                    Vec3i(0,255,0),
                    Vec3i(0,0,255)

                };
                //cout << colors[0] << endl;
                //Vec3f color;
                //利用重心坐标插值z和uv
                for (int i = 0; i < 3; i++) {
                    P.z += pts[i].z * coord[i];
                    T.x += uvs[i].x * coord[i];
                    //cout << "uv.x : " << T.x << endl;
                    T.y += uvs[i].y * coord[i];
                    //cout << "uv.y : " << T.y << endl;
                    /*N.x += norms[i].x * coord[i];
                    N.y += norms[i].y * coord[i];
                    N.z += norms[i].z * coord[i];*/
                    /*color.x += colors[i].x * coord[i];
                    color.y += colors[i].y * coord[i];
                    color.z += colors[i].z * coord[i];*/


                }
               /* if (T.x > 0.9) {
                    cout << "Greater than 1\n";
                }*/

                //cout << color.x << " " << color.y << " " << color.z << endl;

                //cout << "T.x: " << T.x << endl;

                //Texture Sampling
                //w *= T.x;
                w = (int)(w * T.x);
                //cout << "width of the texture: " << w << endl;
                h = (int)(h * T.y);
                //h *= T.y;
                //cout << "height of the texture: " << h << endl;
                TGAColor color = texture.get(w, h);
                

                // TGAColor finalColor(color.x, color.y, color.z, 255);

               

                float x_ratio = (P.x - minx) / (float)(maxx - minx);
                float y_ratio = (P.y - miny) / (float)(maxy - miny);

                //z-buffering
                if (zbuffer[int(P.x + P.y * width)] < P.z) {
                    zbuffer[int(P.x + P.y * width)] = P.z;
                    image.set(P.x, P.y, TGAColor(color.r * intensity, color.g * intensity, color.b * intensity));
                    //image.set(P.x, P.y, finalColor);
                }

                

                ////z-buffering
                //    if (zbuffer[int(P.x + P.y * width)] < P.z) {
                //        zbuffer[int(P.x + P.y * width)] = P.z;
                //        image.set(P.x, P.y, TGAColor(color.r, color.g, color.b));
                //    }

            }
        }
    }


    //Vec3f barycentric(Vec2i* pts, Vec2i P) {
    //    Vec3f u = Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]) ^ Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]);
    //    /* `pts` and `P` has integer value as coordinates
    //       so `abs(u[2])` < 1 means `u[2]` is 0, that means
    //       triangle is degenerate, in this case return something with negative coordinates */
    //    if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
    //    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    //}




