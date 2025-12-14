/**
 * @file trapping_rain.h
 * @brief 接雨水 - 题目说明与声明
 *
 * 双指针经典问题：计算柱状图中能接多少雨水
 *
 * LeetCode 42: Trapping Rain Water
 */

#ifndef TRAPPING_RAIN_H
#define TRAPPING_RAIN_H

#include <vector>

namespace TrappingRainImpl {

/**
 * 题目: 接雨水 (LeetCode 42)
 *
 * 给定 n 个非负整数表示每个柱子的高度，每个柱子的宽度为 1，
 * 计算按此排列的柱子，下雨之后能接多少雨水。
 *
 * 示例:
 *   输入: height = [0,1,0,2,1,0,1,3,2,1,2,1]
 *   输出: 6
 *   解释: 能接 6 个单位的雨水（蓝色部分）
 *
 * 核心思想:
 *   - 每个位置能接的水取决于其左右两边的最高柱子
 *   - water[i] = min(leftMax, rightMax) - height[i]
 *
 * 提示:
 *   - 方法1（双指针）：从两端向中间移动，维护 leftMax 和 rightMax
 *   - 方法2（单调栈）：维护递减栈，遇到更高柱子时计算凹槽
 *
 * 要求:
 *   - 时间复杂度 O(n)
 *   - 空间复杂度 O(1) (双指针法) 或 O(n) (单调栈法)
 */
int trap(std::vector<int>& height);

/**
 * 接雨水 - 单调栈解法
 *
 * 与双指针法的区别：
 * - 双指针：按列计算（垂直方向）
 * - 单调栈：按层计算（横向）
 */
int trapStack(std::vector<int>& height);

// ==================== 测试函数 ====================

void runTests();

} // namespace TrappingRainImpl

#endif // TRAPPING_RAIN_H
