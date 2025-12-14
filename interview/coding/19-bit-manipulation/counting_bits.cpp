/**
 * @file counting_bits.cpp
 * @brief 比特位计数 - 面试者实现
 */

#include "counting_bits.h"

namespace CountingBitsImpl {

// 题目1: 位1的个数 (LeetCode 191)
int hammingWeight(uint32_t n) {
    // TODO: 使用 n & (n-1) 消除最低位的1
    return 0;
}

// 题目2: 比特位计数 (LeetCode 338)
// 返回 0 到 n 每个数的二进制中 1 的个数
std::vector<int> countBits(int n) {
    // TODO: 使用动态规划 dp[i] = dp[i >> 1] + (i & 1)
    return {};
}

} // namespace CountingBitsImpl
