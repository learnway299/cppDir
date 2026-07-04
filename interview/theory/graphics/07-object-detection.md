# 07 - 目标检测 (Object Detection)

## 简介

目标检测用于定位和识别图像中的物体。

---

## 1. 传统方法

### 1.1 级联分类器 (Haar)

```cpp
CascadeClassifier face_cascade;
face_cascade.load("haarcascade_frontalface_default.xml");

vector<Rect> faces;
face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(30, 30));

for (auto& face : faces) {
    rectangle(src, face, Scalar(0, 255, 0), 2);
}
```

### 1.2 HOG + SVM

```cpp
// HOG 描述子
HOGDescriptor hog;
hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

vector<Rect> detections;
hog.detectMultiScale(img, detections);
```

| 步骤 | 说明 |
|------|------|
| 1 | 计算梯度 |
| 2 | 划分 cell |
| 3 | 构建 block 直方图 |
| 4 | 归一化 |
| 5 | SVM 分类 |

---

## 2. 深度学习方法

### 2.1 检测算法对比

| 算法 | 特点 | 速度 | 精度 |
|------|------|------|------|
| R-CNN | 选择性搜索 | 慢 | 高 |
| Fast R-CNN | ROI Pooling | 中 | 高 |
| Faster R-CNN | RPN | 中 | 高 |
| YOLO | 单阶段 | 快 | 中 |
| SSD | 多尺度 | 快 | 中 |

### 2.2 YOLO 原理

```
输入图像
    │
    ▼
CNN 特征提取
    │
    ▼
划分网格 (7×7)
    │
    ▼
每格预测：
- B 个边界框 (x, y, w, h, confidence)
- C 个类别概率
    │
    ▼
NMS 过滤
    │
    ▼
最终检测结果
```

---

## 3. OpenCV DNN

```cpp
// 加载模型
Net net = readNetFromDarknet("yolov3.cfg", "yolov3.weights");
net.setPreferableBackend(DNN_BACKEND_CUDA);
net.setPreferableTarget(DNN_TARGET_CUDA);

// 前向推理
Mat blob = blobFromImage(img, 1/255.0, Size(416, 416), Scalar(), true, false);
net.setInput(blob);
vector<Mat> outs;
net.forward(outs, getOutputsNames(net));

// 解析结果
// ... NMS 处理
```

---

## 4. 面试常见问题

**Q1：YOLO 为什么快？**
- 单阶段检测，一次前向传播
- 不需要区域提议

**Q2：NMS 是什么？**
- 非极大值抑制
- 保留置信度最高的框
- 删除高重叠的框

**Q3：Anchor 是什么？**
- 预定义的候选框尺寸
- 网络预测相对于 anchor 的偏移
