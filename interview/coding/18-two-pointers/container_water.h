/**
 * @file container_water.h
 * @brief 盛最多水的容器 - 题目说明与声明
 *
 * 双指针经典问题：找两条线围成的最大面积
 *
 * LeetCode 11: Container With Most Water
 */

#ifndef CONTAINER_WATER_H
#define CONTAINER_WATER_H

#include <vector>

namespace ContainerWaterImpl {

/**
 * 题目: 盛最多水的容器 (LeetCode 11)
 *
 * 给定 n 个非负整数 height，每个数代表一条垂直线段的高度。
 * 找出两条线段，使其与 x 轴共同构成的容器可以容纳最多的水。
 *
 * 示例:
 *   输入: height = [1,8,6,2,5,4,8,3,7]
 *   输出: 49
 *   解释: 高度为 8 和 7 的两条线段，距离为 7，面积 = min(8,7) * 7 = 49
 *
 * 提示:
 *   - 面积 = min(height[left], height[right]) * (right - left)
 *   - 双指针从两端向中间移动
 *   - 每次移动较矮的一边（因为移动较高的边面积只会变小）
 *
 * 要求:
 *   - 时间复杂度 O(n)
 *   - 空间复杂度 O(1)
 */
int maxArea(std::vector<int>& height);

// ==================== 测试函数 ====================

void runTests();

} // namespace ContainerWaterImpl

#endif // CONTAINER_WATER_H
