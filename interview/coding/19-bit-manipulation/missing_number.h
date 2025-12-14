/**
 * @file missing_number.h
 * @brief 丢失的数字 - 面试题
 */

#ifndef MISSING_NUMBER_H
#define MISSING_NUMBER_H

#include <vector>

namespace MissingNumberImpl {

/**
 * 题目: 丢失的数字 (LeetCode 268)
 * 给定一个包含 [0, n] 中 n 个数的数组 nums，找出 [0, n] 这个范围内没有出现在数组中的那个数。
 *
 * 示例:
 * 输入: nums = [3,0,1]
 * 输出: 2
 * 解释: n = 3，因为有 3 个数字，所以所有的数字都在范围 [0,3] 内。2 是丢失的数字，因为它没有出现在 nums 中。
 *
 * 提示: 可以使用异或运算或者数学公式
 */
int missingNumber(std::vector<int>& nums);

/**
 * 方法2: 数学公式法
 * 利用等差数列求和公式
 */
int missingNumberSum(std::vector<int>& nums);

// 测试函数
void runTests();

} // namespace MissingNumberImpl

#endif // MISSING_NUMBER_H
