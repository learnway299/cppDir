# 05 - 图像处理 (Image Processing)

## 简介

图像处理的核心技术：滤波、边缘检测、形态学操作等。

---

## 1. 滤波

### 1.1 常用滤波器

| 滤波器 | 特点 | 用途 |
|--------|------|------|
| 均值滤波 | 简单模糊 | 去噪 |
| 高斯滤波 | 加权模糊 | 去噪，保留边缘 |
| 中值滤波 | 非线性 | 去椒盐噪声 |
| 双边滤波 | 保边去噪 | 美颜 |

```cpp
// 均值滤波
blur(src, dst, Size(5, 5));

// 高斯滤波
GaussianBlur(src, dst, Size(5, 5), 1.5);

// 中值滤波
medianBlur(src, dst, 5);

// 双边滤波
bilateralFilter(src, dst, 9, 75, 75);
```

---

## 2. 边缘检测

### 2.1 Sobel 算子

```cpp
// 梯度
Sobel(gray, gradX, CV_16S, 1, 0);  // X 方向
Sobel(gray, gradY, CV_16S, 0, 1);  // Y 方向

// 幅值
Mat magnitude;
convertScaleAbs(gradX, gradX);
convertScaleAbs(gradY, gradY);
addWeighted(gradX, 0.5, gradY, 0.5, 0, magnitude);
```

### 2.2 Canny 边缘检测

```cpp
// Canny 算法
// 1. 高斯滤波
// 2. 计算梯度
// 3. 非极大值抑制
// 4. 双阈值检测
// 5. 边缘连接

Canny(gray, edges, 50, 150);  // 低阈值, 高阈值
```

---

## 3. 形态学操作

```cpp
Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

// 腐蚀：缩小白色区域
erode(src, dst, kernel);

// 膨胀：扩大白色区域
dilate(src, dst, kernel);

// 开运算：先腐蚀后膨胀（去噪点）
morphologyEx(src, dst, MORPH_OPEN, kernel);

// 闭运算：先膨胀后腐蚀（填小孔）
morphologyEx(src, dst, MORPH_CLOSE, kernel);
```

---

## 4. 直方图

```cpp
// 计算直方图
vector<Mat> bgr_planes;
split(src, bgr_planes);

int histSize = 256;
float range[] = {0, 256};
const float* histRange = {range};

Mat hist;
calcHist(&bgr_planes[0], 1, 0, Mat(), hist, 1, &histSize, &histRange);

// 直方图均衡化
equalizeHist(gray, equalized);
```

---

## 5. 面试常见问题

**Q1：高斯滤波和均值滤波区别？**
- 均值：权重相同，简单模糊
- 高斯：中心权重大，保留更多细节

**Q2：Canny 边缘检测步骤？**
1. 高斯滤波去噪
2. Sobel 计算梯度
3. 非极大值抑制
4. 双阈值检测
5. 边缘连接

**Q3：开运算和闭运算用途？**
- 开运算：去除小噪点
- 闭运算：填充小孔洞
