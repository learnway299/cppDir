/**
 * @file two_sum_sorted.h
 * @brief 两数之和（有序数组）- 题目说明与声明
 *
 * 双指针经典问题：在有序数组中找两个数使其和等于目标值
 *
 * LeetCode 167: Two Sum II - Input Array Is Sorted
 */

#ifndef TWO_SUM_SORTED_H
#define TWO_SUM_SORTED_H

#include <vector>

namespace TwoSumSortedImpl {

/**
 * 题目: 两数之和 II (LeetCode 167)
 *
 * 给定一个已按升序排列的整数数组 numbers，
 * 从数组中找出两个数满足相加之和等于目标数 target。
 * 返回两个数的下标（1-indexed）。
 *
 * 示例:
 *   输入: numbers = [2,7,11,15], target = 9
 *   输出: [1,2]
 *   解释: 2 + 7 = 9
 *
 * 要求:
 *   - 时间复杂度 O(n)
 *   - 空间复杂度 O(1)
 *
 * 提示:
 *   - 利用数组有序的特性
 *   - 双指针从两端向中间移动
 */
std::vector<int> twoSum(std::vector<int>& numbers, int target);

// ==================== 测试函数 ====================

void runTests();

} // namespace TwoSumSortedImpl

#endif // TWO_SUM_SORTED_H
