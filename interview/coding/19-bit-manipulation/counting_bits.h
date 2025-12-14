/**
 * @file counting_bits.h
 * @brief 比特位计数 - 面试题
 */

#ifndef COUNTING_BITS_H
#define COUNTING_BITS_H

#include <vector>
#include <cstdint>

namespace CountingBitsImpl {

/**
 * 题目1: 位1的个数 (LeetCode 191 - Hamming Weight)
 * 编写一个函数，输入是一个无符号整数，返回其二进制表达式中数字位数为 '1' 的个数。
 *
 * 示例:
 * 输入: 11 (二进制 1011)
 * 输出: 3
 * 解释: 整数 11 的二进制表示为 1011，其中有 3 个 '1'。
 *
 * 提示: 使用 n & (n-1) 消除最低位的1
 */
int hammingWeight(uint32_t n);

/**
 * 题目2: 比特位计数 (LeetCode 338)
 * 给你一个整数 n，对于 0 <= i <= n 中的每个 i，
 * 计算其二进制表示中 1 的个数，返回一个长度为 n + 1 的数组 ans 作为答案。
 *
 * 示例:
 * 输入: n = 5
 * 输出: [0,1,1,2,1,2]
 * 解释:
 * 0 --> 0
 * 1 --> 1
 * 2 --> 1
 * 3 --> 2
 * 4 --> 1
 * 5 --> 2
 *
 * 提示: 使用动态规划 dp[i] = dp[i >> 1] + (i & 1)
 */
std::vector<int> countBits(int n);

// 测试函数
void runTests();

} // namespace CountingBitsImpl

#endif // COUNTING_BITS_H
