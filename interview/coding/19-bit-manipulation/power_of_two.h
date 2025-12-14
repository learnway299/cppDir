/**
 * @file power_of_two.h
 * @brief 2的幂 - 面试题
 */

#ifndef POWER_OF_TWO_H
#define POWER_OF_TWO_H

namespace PowerOfTwoImpl {

/**
 * 题目1: 2的幂 (LeetCode 231)
 * 给定一个整数，判断它是否是2的幂次方。
 *
 * 示例:
 * 输入: 16
 * 输出: true (2^4 = 16)
 *
 * 输入: 3
 * 输出: false
 *
 * 提示: 2的幂的二进制表示只有一个1，使用 n & (n-1) 技巧
 */
bool isPowerOfTwo(int n);

/**
 * 题目2: 4的幂 (LeetCode 342)
 * 给定一个整数，判断它是否是4的幂次方。
 *
 * 示例:
 * 输入: 16
 * 输出: true (4^2 = 16)
 *
 * 输入: 8
 * 输出: false (是2的幂但不是4的幂)
 *
 * 提示: 首先是2的幂，且1在奇数位上（使用0x55555555掩码）
 */
bool isPowerOfFour(int n);

// 测试函数
void runTests();

} // namespace PowerOfTwoImpl

#endif // POWER_OF_TWO_H
