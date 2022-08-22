#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

/*istringstream�Ĺ����߼���
* �����Ȼ�ȡһ���ַ������г�ʼ��  std::istringstream iss(line.c_str());
* ʹ��>>�������iss�ж�ȡ�ַ� iss >> trash
* ÿ�������ո�ʱ����ȡ���Զ�ֹͣ
* ���Զ��ʹ��>>����ͬ���ַ���ȡ����ͬ�ı�����  iss >> idx >> trash >> itrash >> trash >> itrash
* �Զ�ʶ��ʲô�ַ��÷ŵ�ʲô������ ��trash���char����Ӧб�ܣ�itrash��idxΪint��������Ӧ����
*/


Model::Model(const char* filename) : verts_(), faces_(), uvs_() {
    std::ifstream in;
    //����Ӧ�ļ� ʹ��ֻ��ģʽ
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        //��ȡһ�д浽line��
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash; //trash�ַ���������õ�v��f��ͷ�Լ�б��"/"
        //compare()������ �Ƚ���һ�д��±�0��ʼ��2���ַ��ǲ���"v "��Ҳ���Ǵ�����
        //��ͬʱ����0������ȡ��
        if (!line.compare(0, 2, "v ")) {
            iss >> trash; //��v��ŵ�trash��
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i]; //���ζ�ȡxyzֵ
            verts_.push_back(v); //��ӵ�����������
        }
        //��������
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec2f uv;
            for (int i = 0; i < 2; i++) iss >> uv.raw[i];
            uvs_.push_back(uv);
        }
        //������
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
            //��iss�����ζ�ȡ 
            //index��������Ҫ�Ķ��������� /��û�õ�б�ܣ� texcoord����ʱ����Ҫ���������꣬����itrash��/��û�õ�б�ܣ� normal(��ʱ����Ҫ�ķ�����)
            while (iss >> idx >> trash >> iuv >> trash >> inorm) {
                idx--; // in wavefront obj all indices start at 1, not zero
                iuv--;
                inorm--;
                f.push_back(idx); //���������idx��ɵ�vector<int> ֮���Բ�ֱ����Vec3����Ϊ�����п��ܲ�ֹ������ͼԪ���ı��Σ�
                f.push_back(iuv);
                f.push_back(inorm);
                //f������������Ӧ�ú�obj��ʽ��һ������Ϊ���飬ÿһ�鶼��Ӧ  ����/����/����
            }
            faces_.push_back(f); //��ӵ���������
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