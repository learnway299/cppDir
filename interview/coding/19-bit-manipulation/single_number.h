/**
 * @file single_number.h
 * @brief 只出现一次的数字 - 面试题
 */

#ifndef SINGLE_NUMBER_H
#define SINGLE_NUMBER_H

#include <vector>

namespace SingleNumberImpl {

/**
 * 题目1: 只出现一次的数字 (LeetCode 136)
 * 给定一个非空整数数组，除了某个元素只出现一次以外，其余每个元素均出现两次。找出那个只出现了一次的元素。
 *
 * 示例:
 * 输入: [2,2,1]
 * 输出: 1
 *
 * 提示: 使用异或运算的性质 a ^ a = 0, a ^ 0 = a
 */
int singleNumber(std::vector<int>& nums);

/**
 * 题目2: 只出现一次的数字 II (LeetCode 137)
 * 给定一个非空整数数组，除了某个元素只出现一次以外，其余每个元素均出现三次。找出那个只出现了一次的元素。
 *
 * 示例:
 * 输入: [2,2,3,2]
 * 输出: 3
 *
 * 提示: 统计每一位上1出现的次数，对3取模
 */
int singleNumber2(std::vector<int>& nums);

/**
 * 题目3: 只出现一次的数字 III (LeetCode 260)
 * 给定一个整数数组，其中恰好有两个元素只出现一次，其余所有元素均出现两次。找出只出现一次的那两个元素。
 *
 * 示例:
 * 输入: [1,2,1,3,2,5]
 * 输出: [3,5]
 *
 * 提示: 先异或得到两个数的异或结果，然后利用lowbit分组
 */
std::vector<int> singleNumber3(std::vector<int>& nums);

// 测试函数
void runTests();

} // namespace SingleNumberImpl

#endif // SINGLE_NUMBER_H
