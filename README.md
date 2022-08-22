# TinyRenderer_Modified

基于ssloy大神的TinyRenderer教程，参考其中的思想，自己重构了一下代码的产物，就当是自己练习的软件光栅化渲染器啦。

主要是在这个过程中再次感受一下整个渲染管线的运作方式

目前实现阶段：

![image-20220822210735095](https://pic.imgdb.cn/item/630381ea16f2c2beb1187ed7.png)

- 实现透视投影（仿照OpenGL），自建LookAt和Perspertive，构建观察和投影矩阵，以及应用透视除法
- Z-Buffer
- 纹理映射

下一步准备继续完善项目

- Shader support
- Transform Normals
- Lighting
- SSAO
- ....
