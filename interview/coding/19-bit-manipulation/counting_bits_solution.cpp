/**
 * @file counting_bits_solution.cpp
 * @brief 比特位计数 - 参考解答
 */

#include "counting_bits.h"
#include <iostream>
#include <cassert>

namespace CountingBitsImpl {
namespace Solution {

// ==================== 题目1: 位1的个数 (Hamming Weight) ====================
int hammingWeight(uint32_t n) {
    int count = 0;
    while (n) {
        n &= (n - 1);  // 消除最低位的1
        count++;
    }
    return count;
}

// ==================== 题目2: 比特位计数 ====================
// dp[i] = dp[i >> 1] + (i & 1)
std::vector<int> countBits(int n) {
    std::vector<int> dp(n + 1);
    for (int i = 1; i <= n; ++i) {
        dp[i] = dp[i >> 1] + (i & 1);
    }
    return dp;
}

} // namespace Solution

// ==================== 测试函数 ====================
void runTests() {
    std::cout << "=== Counting Bits Solution ===" << std::endl;

    // 测试 Hamming Weight
    {
        assert(Solution::hammingWeight(11) == 3);   // 1011 -> 3个1
        assert(Solution::hammingWeight(128) == 1);  // 10000000 -> 1个1
        assert(Solution::hammingWeight(0) == 0);
    }
    std::cout << "  Hamming Weight: PASSED" << std::endl;

    // 测试比特位计数
    {
        auto bits = Solution::countBits(5);
        assert((bits == std::vector<int>{0, 1, 1, 2, 1, 2}));
    }
    std::cout << "  Count Bits: PASSED" << std::endl;
}

} // namespace CountingBitsImpl

/**
 * 关键要点：
 *
 * 1. Brian Kernighan 算法：
 *    - n & (n-1) 消除最低位的 1
 *    - 循环次数 = 1 的个数
 *    - 时间复杂度 O(k)，k 是 1 的个数
 *
 * 2. 动态规划计算比特位：
 *    - dp[i] = dp[i >> 1] + (i & 1)
 *    - i >> 1 是 i 去掉最低位
 *    - i & 1 是最低位
 *
 * 3. 其他方法：
 *    - 查表法：预处理 256 个数的比特数
 *    - 内置函数：__builtin_popcount()
 *
 * 4. 时间复杂度：
 *    - hammingWeight: O(k)
 *    - countBits: O(n)
 */
