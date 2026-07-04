# 06 - 特征与匹配 (Feature Matching)

## 简介

特征检测和匹配是计算机视觉的核心技术。

---

## 1. 特征检测器

### 1.1 Harris 角点

```cpp
Mat dst;
cornerHarris(gray, dst, 2, 3, 0.04);

// 标记角点
for (int i = 0; i < dst.rows; i++) {
    for (int j = 0; j < dst.cols; j++) {
        if (dst.at<float>(i, j) > threshold) {
            circle(src, Point(j, i), 5, Scalar(0, 0, 255), 2);
        }
    }
}
```

### 1.2 SIFT/SURF/ORB

| 算法 | 特点 | 速度 |
|------|------|------|
| SIFT | 尺度不变，精度高 | 慢 |
| SURF | 加速版 SIFT | 中 |
| ORB | 快速，免费 | 快 |

```cpp
// ORB 检测
Ptr<ORB> orb = ORB::create();
vector<KeyPoint> keypoints;
Mat descriptors;
orb->detectAndCompute(gray, Mat(), keypoints, descriptors);

// 绘制关键点
drawKeypoints(src, keypoints, dst, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
```

---

## 2. 特征匹配

```cpp
// 暴力匹配
BFMatcher matcher(NORM_HAMMING);  // ORB 用 HAMMING
vector<DMatch> matches;
matcher.match(descriptors1, descriptors2, matches);

// FLANN 匹配（更快）
Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
vector<vector<DMatch>> knnMatches;
matcher->knnMatch(descriptors1, descriptors2, knnMatches, 2);

// 比率测试
vector<DMatch> goodMatches;
for (auto& m : knnMatches) {
    if (m[0].distance < 0.7 * m[1].distance) {
        goodMatches.push_back(m[0]);
    }
}
```

---

## 3. RANSAC

**RANSAC**：随机采样一致性，剔除错误匹配。

```cpp
// 计算单应矩阵
vector<Point2f> pts1, pts2;
for (auto& m : goodMatches) {
    pts1.push_back(keypoints1[m.queryIdx].pt);
    pts2.push_back(keypoints2[m.trainIdx].pt);
}

Mat H = findHomography(pts1, pts2, RANSAC);

// 透视变换
warpPerspective(src, dst, H, dst.size());
```

---

## 4. 面试常见问题

**Q1：SIFT 和 ORB 如何选择？**
- SIFT：精度要求高，速度不敏感
- ORB：实时应用，对精度要求不高

**Q2：什么是比率测试？**
- 最近距离 / 次近距离 < 阈值
- 过滤模糊匹配

**Q3：RANSAC 原理？**
- 随机采样计算模型
- 统计内点数
- 迭代找最优模型
