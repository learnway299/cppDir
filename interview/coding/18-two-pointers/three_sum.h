/**
 * @file three_sum.h
 * @brief 三数之和 - 题目说明与声明
 *
 * 双指针经典问题：找出数组中和为 0 的所有三元组
 *
 * LeetCode 15: 3Sum
 * LeetCode 16: 3Sum Closest
 */

#ifndef THREE_SUM_H
#define THREE_SUM_H

#include <vector>

namespace ThreeSumImpl {

/**
 * 题目1: 三数之和 (LeetCode 15)
 *
 * 给定一个整数数组 nums，找出所有和为 0 的三元组。
 * 答案中不可包含重复的三元组。
 *
 * 示例:
 *   输入: nums = [-1,0,1,2,-1,-4]
 *   输出: [[-1,-1,2],[-1,0,1]]
 *
 * 提示:
 *   - 先排序
 *   - 固定第一个数，对剩余数组用双指针
 *   - 注意去重
 */
std::vector<std::vector<int>> threeSum(std::vector<int>& nums);

/**
 * 题目2: 最接近的三数之和 (LeetCode 16)
 *
 * 找出与 target 最接近的三数之和。
 *
 * 示例:
 *   输入: nums = [-1,2,1,-4], target = 1
 *   输出: 2
 *   解释: -1 + 2 + 1 = 2
 */
int threeSumClosest(std::vector<int>& nums, int target);

// ==================== 测试函数 ====================

void runTests();

} // namespace ThreeSumImpl

#endif // THREE_SUM_H
