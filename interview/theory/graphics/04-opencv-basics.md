# 04 - OpenCV 基础 (OpenCV Basics)

## 简介

OpenCV 是计算机视觉库，用于图像处理和视觉分析。

---

## 1. Mat 结构

```cpp
#include <opencv2/opencv.hpp>
using namespace cv;

// 创建 Mat
Mat img = imread("image.jpg");           // 读取图片
Mat zeros = Mat::zeros(480, 640, CV_8UC3); // 全黑
Mat ones = Mat::ones(480, 640, CV_8UC1);   // 全 1

// Mat 属性
int rows = img.rows;          // 高度
int cols = img.cols;          // 宽度
int channels = img.channels(); // 通道数
int type = img.type();        // 类型（如 CV_8UC3）

// 访问像素
Vec3b pixel = img.at<Vec3b>(y, x);  // BGR
uchar blue = pixel[0];
uchar green = pixel[1];
uchar red = pixel[2];
```

---

## 2. 图像读写

```cpp
// 读取
Mat img = imread("image.jpg", IMREAD_COLOR);     // 彩色
Mat gray = imread("image.jpg", IMREAD_GRAYSCALE); // 灰度

// 保存
imwrite("output.png", img);

// 显示
imshow("Window", img);
waitKey(0);  // 等待按键
```

---

## 3. 颜色空间

```cpp
// BGR → Gray
cvtColor(src, gray, COLOR_BGR2GRAY);

// BGR → HSV
cvtColor(src, hsv, COLOR_BGR2HSV);

// BGR → RGB
cvtColor(src, rgb, COLOR_BGR2RGB);
```

| 颜色空间 | 用途 |
|----------|------|
| BGR | OpenCV 默认 |
| RGB | 通用显示 |
| HSV | 颜色过滤 |
| Lab | 颜色相似度 |
| YCrCb | 肤色检测 |

---

## 4. ROI 与通道

```cpp
// ROI（感兴趣区域）
Mat roi = img(Rect(x, y, width, height));

// 通道分离
vector<Mat> channels;
split(img, channels);

// 通道合并
Mat merged;
merge(channels, merged);
```

---

## 5. 基本操作

```cpp
// 调整大小
resize(src, dst, Size(640, 480));

// 翻转
flip(src, dst, 1);  // 0:垂直, 1:水平, -1:两者

// 旋转
rotate(src, dst, ROTATE_90_CLOCKWISE);

// 阈值化
threshold(gray, binary, 127, 255, THRESH_BINARY);
```

---

## 6. 面试常见问题

**Q1：Mat 的数据类型含义？**
- CV_8UC3：8 位无符号，3 通道
- CV_32FC1：32 位浮点，单通道

**Q2：OpenCV 为什么用 BGR？**
- 历史原因：早期设备使用 BGR 顺序
- 需要注意与 RGB 的转换

**Q3：深拷贝和浅拷贝？**
- `Mat b = a;`：浅拷贝，共享数据
- `a.copyTo(b);`：深拷贝，数据独立
