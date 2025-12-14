/**
 * @file container_water.cpp
 * @brief 盛最多水的容器 - 面试者实现文件
 */

#include "container_water.h"

namespace ContainerWaterImpl {

int maxArea(std::vector<int>& height) {
    // TODO: 使用双指针找到最大面积
    // 提示:
    // 1. 初始化 left = 0, right = size - 1, maxWater = 0
    // 2. 当 left < right 时循环：
    //    - 计算当前面积 = min(height[left], height[right]) * (right - left)
    //    - 更新 maxWater
    //    - 移动较矮的一边（height[left] < height[right] ? left++ : right--）
    // 3. 返回 maxWater
    return 0;
}

} // namespace ContainerWaterImpl
