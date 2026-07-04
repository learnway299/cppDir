# Graphics & Image Processing Knowledge

图形图像知识 - OpenGL 与 OpenCV 面试必备的 8 个主题。

---

## 知识体系

### OpenGL 图形学 - 3 个主题

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 01 | [图形学基础](01-graphics-fundamentals.md) | 坐标系、变换矩阵、齐次坐标、投影变换 |
| 02 | [OpenGL 基础](02-opengl-basics.md) | 渲染管线、着色器、VAO/VBO/EBO、纹理 |
| 03 | [OpenGL 进阶](03-opengl-advanced.md) | 光照模型、阴影、帧缓冲、后处理 |

### OpenCV 图像处理 - 5 个主题

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 04 | [OpenCV 基础](04-opencv-basics.md) | Mat 结构、图像读写、颜色空间、ROI |
| 05 | [图像处理](05-image-processing.md) | 滤波、边缘检测、形态学、直方图 |
| 06 | [特征与匹配](06-feature-matching.md) | SIFT/ORB/FAST、特征匹配、RANSAC |
| 07 | [目标检测](07-object-detection.md) | 级联分类器、HOG+SVM、深度学习检测 |
| 08 | [3D 视觉](08-3d-vision.md) | 相机标定、立体视觉、点云处理 |

---

## 面试重点

### 图形学必问

1. **渲染管线**
   - 顶点着色器 → 图元装配 → 光栅化 → 片段着色器
   - 各阶段功能

2. **坐标变换**
   - 模型矩阵、视图矩阵、投影矩阵
   - MVP 变换

3. **光照模型**
   - Phong 光照
   - 法线贴图

### 图像处理必问

1. **滤波**
   - 均值滤波、高斯滤波、中值滤波
   - 边缘检测（Sobel、Canny）

2. **特征提取**
   - Harris 角点
   - SIFT/SURF/ORB 描述子

3. **相机标定**
   - 内参、外参、畸变参数

---

## 文件列表

```
graphics/
├── README.md                    # 本文件
├── 01-graphics-fundamentals.md  # 图形学基础
├── 02-opengl-basics.md          # OpenGL 基础
├── 03-opengl-advanced.md        # OpenGL 进阶
├── 04-opencv-basics.md          # OpenCV 基础
├── 05-image-processing.md       # 图像处理
├── 06-feature-matching.md       # 特征与匹配
├── 07-object-detection.md       # 目标检测
└── 08-3d-vision.md              # 3D 视觉
```
