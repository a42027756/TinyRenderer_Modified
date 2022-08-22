#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

/*istringstream的工作逻辑：
* 它首先获取一串字符串进行初始化  std::istringstream iss(line.c_str());
* 使用>>运算符从iss中读取字符 iss >> trash
* 每次遇到空格时，读取会自动停止
* 可以多次使用>>将不同的字符读取到不同的变量中  iss >> idx >> trash >> itrash >> trash >> itrash
* 自动识别什么字符该放到什么变量中 如trash存放char，对应斜杠；itrash和idx为int变量，对应数字
*/


Model::Model(const char* filename) : verts_(), faces_(), uvs_() {
    std::ifstream in;
    //打开相应文件 使用只读模式
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        //读取一行存到line中
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash; //trash字符，存放无用的v和f开头以及斜杠"/"
        //compare()函数： 比较这一行从下标0开始的2个字符是不是"v "，也就是代表顶点
        //相同时返回0，所以取反
        if (!line.compare(0, 2, "v ")) {
            iss >> trash; //将v存放到trash中
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i]; //依次读取xyz值
            verts_.push_back(v); //添加到顶点数组中
        }
        //纹理坐标
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec2f uv;
            for (int i = 0; i < 2; i++) iss >> uv.raw[i];
            uvs_.push_back(uv);
        }
        //法向量
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f normal;
            for (int i = 0; i < 3; i++) iss >> normal.raw[i];
            normals_.push_back(normal);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itrash, idx, iuv, inorm;
            iss >> trash;
            //从iss中依次读取 
            //index（我们想要的顶点索引） /（没用的斜杠） texcoord（暂时不需要的纹理坐标，放在itrash）/（没用的斜杠） normal(暂时不需要的法向量)
            while (iss >> idx >> trash >> iuv >> trash >> inorm) {
                idx--; // in wavefront obj all indices start at 1, not zero
                iuv--;
                inorm--;
                f.push_back(idx); //组成由三个idx组成的vector<int> 之所以不直接用Vec3是因为索引有可能不止三个（图元是四边形）
                f.push_back(iuv);
                f.push_back(inorm);
                //f数组的组成最终应该和obj格式中一样，分为三组，每一组都对应  顶点/纹理/法线
            }
            faces_.push_back(f); //添加到面数组中
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << uvs_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::uv(int i) {
    return uvs_[i];
}

Vec3f Model::normal(int i) {
    return normals_[i];
}

//mklink /D "C:\Users\HP\Appdata\Roaming\Apple Computer\MobileSync\Backup" "D:\Backup"