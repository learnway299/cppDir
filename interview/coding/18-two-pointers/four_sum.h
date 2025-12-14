/**
 * @file four_sum.h
 * @brief 四数之和 - 题目说明与声明
 *
 * 双指针经典问题：找出数组中和为目标值的所有四元组
 *
 * LeetCode 18: 4Sum
 */

#ifndef FOUR_SUM_H
#define FOUR_SUM_H

#include <vector>

namespace FourSumImpl {

/**
 * 题目: 四数之和 (LeetCode 18)
 *
 * 给定一个整数数组 nums 和一个目标值 target，
 * 找出所有和为 target 的四元组。
 * 答案中不可包含重复的四元组。
 *
 * 示例:
 *   输入: nums = [1,0,-1,0,-2,2], target = 0
 *   输出: [[-2,-1,1,2],[-2,0,0,2],[-1,0,0,1]]
 *
 * 提示:
 *   - 先排序
 *   - 两层循环固定前两个数，对剩余数组用双指针
 *   - 注意去重（三个位置都要去重）
 *   - 注意溢出问题（使用 long long）
 *
 * 要求:
 *   - 时间复杂度 O(n^3)
 *   - 空间复杂度 O(1)
 */
std::vector<std::vector<int>> fourSum(std::vector<int>& nums, int target);

// ==================== 测试函数 ====================

void runTests();

} // namespace FourSumImpl

#endif // FOUR_SUM_H
