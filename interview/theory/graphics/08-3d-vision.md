# 08 - 3D 视觉 (3D Vision)

## 简介

3D 视觉涉及相机标定、立体视觉和三维重建。

---

## 1. 相机模型

### 1.1 针孔相机模型

```
世界坐标 ─► 相机坐标 ─► 图像坐标 ─► 像素坐标

P_pixel = K × [R|t] × P_world

K = | fx  0  cx |   内参矩阵
    | 0  fy  cy |
    | 0   0   1 |

[R|t] = 外参（旋转 + 平移）
```

### 1.2 畸变模型

```
径向畸变：r² = x² + y²
x_d = x(1 + k1r² + k2r⁴ + k3r⁶)
y_d = y(1 + k1r² + k2r⁴ + k3r⁶)

切向畸变：
x_d += 2p1xy + p2(r² + 2x²)
y_d += p1(r² + 2y²) + 2p2xy
```

---

## 2. 相机标定

```cpp
// 标定流程
vector<vector<Point3f>> objectPoints;  // 世界坐标
vector<vector<Point2f>> imagePoints;   // 图像坐标

// 检测棋盘格角点
vector<Point2f> corners;
bool found = findChessboardCorners(gray, patternSize, corners);
cornerSubPix(gray, corners, Size(11,11), Size(-1,-1), criteria);

// 标定
Mat cameraMatrix, distCoeffs;
vector<Mat> rvecs, tvecs;
calibrateCamera(objectPoints, imagePoints, imageSize,
                cameraMatrix, distCoeffs, rvecs, tvecs);

// 去畸变
undistort(src, dst, cameraMatrix, distCoeffs);
```

---

## 3. 立体视觉

### 3.1 极线几何

```
左图像点 p1 ←→ 右图像极线 l2
右图像点 p2 ←→ 左图像极线 l1

基础矩阵：p2ᵀ F p1 = 0
本质矩阵：E = K2ᵀ F K1
```

### 3.2 立体匹配

```cpp
// 创建立体匹配器
Ptr<StereoSGBM> stereo = StereoSGBM::create(
    0,      // minDisparity
    64,     // numDisparities
    3       // blockSize
);

Mat disparity;
stereo->compute(leftGray, rightGray, disparity);

// 深度计算
// depth = baseline * focal / disparity
```

---

## 4. 点云处理

```cpp
// 视差图转点云
Mat points3D;
reprojectImageTo3D(disparity, points3D, Q);

// PCL 点云处理
#include <pcl/point_cloud.h>
#include <pcl/filters/voxel_grid.h>

// 下采样
pcl::VoxelGrid<pcl::PointXYZ> filter;
filter.setLeafSize(0.01f, 0.01f, 0.01f);
filter.filter(*cloud_filtered);
```

---

## 5. 面试常见问题

**Q1：相机内参包含什么？**
- 焦距 (fx, fy)
- 主点 (cx, cy)
- 畸变系数

**Q2：如何计算深度？**
- 双目：depth = baseline × focal / disparity
- 需要标定的立体相机

**Q3：点云配准方法？**
- ICP（迭代最近点）
- NDT（正态分布变换）
- 特征匹配
