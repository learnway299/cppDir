/**
 * @file reverse_bits_solution.cpp
 * @brief 颠倒二进制位 - 参考解答
 */

#include "reverse_bits.h"
#include <iostream>
#include <cassert>

namespace ReverseBitsImpl {
namespace Solution {

// ==================== 方法1: 逐位翻转 ====================
uint32_t reverseBits(uint32_t n) {
    uint32_t result = 0;
    for (int i = 0; i < 32; ++i) {
        result = (result << 1) | (n & 1);
        n >>= 1;
    }
    return result;
}

// ==================== 方法2: 分治法 ====================
uint32_t reverseBitsDivide(uint32_t n) {
    n = ((n & 0xffff0000) >> 16) | ((n & 0x0000ffff) << 16);
    n = ((n & 0xff00ff00) >> 8)  | ((n & 0x00ff00ff) << 8);
    n = ((n & 0xf0f0f0f0) >> 4)  | ((n & 0x0f0f0f0f) << 4);
    n = ((n & 0xcccccccc) >> 2)  | ((n & 0x33333333) << 2);
    n = ((n & 0xaaaaaaaa) >> 1)  | ((n & 0x55555555) << 1);
    return n;
}

} // namespace Solution

// ==================== 测试函数 ====================
void runTests() {
    std::cout << "=== Reverse Bits Solution ===" << std::endl;

    // 测试逐位翻转
    {
        // 43261596 -> 964176192
        assert(Solution::reverseBits(43261596) == 964176192);
    }
    std::cout << "  Reverse Bits (iterative): PASSED" << std::endl;

    // 测试分治法
    {
        assert(Solution::reverseBitsDivide(43261596) == 964176192);
    }
    std::cout << "  Reverse Bits (divide): PASSED" << std::endl;

    // 验证两种方法一致
    {
        uint32_t test = 0xABCD1234;
        assert(Solution::reverseBits(test) == Solution::reverseBitsDivide(test));
    }
    std::cout << "  Both methods consistent: PASSED" << std::endl;
}

} // namespace ReverseBitsImpl

/**
 * 关键要点：
 *
 * 1. 逐位翻转：
 *    - 每次取最低位，放到结果的最高位
 *    - 循环 32 次
 *    - 时间复杂度 O(32)
 *
 * 2. 分治法：
 *    - 先交换高16位和低16位
 *    - 再交换每8位
 *    - 再交换每4位
 *    - 再交换每2位
 *    - 最后交换相邻位
 *    - 时间复杂度 O(1)
 *
 * 3. 掩码说明：
 *    - 0xffff0000 / 0x0000ffff: 16位交换
 *    - 0xff00ff00 / 0x00ff00ff: 8位交换
 *    - 0xf0f0f0f0 / 0x0f0f0f0f: 4位交换
 *    - 0xcccccccc / 0x33333333: 2位交换
 *    - 0xaaaaaaaa / 0x55555555: 1位交换
 */
