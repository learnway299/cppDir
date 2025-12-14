/**
 * @file bitwise_and_range_solution.cpp
 * @brief 数字范围按位与 - 参考解答
 */

#include "bitwise_and_range.h"
#include <iostream>
#include <cassert>

namespace BitwiseAndRangeImpl {
namespace Solution {

// ==================== 方法1: 找公共前缀 ====================
// 不断右移直到相等，因为范围内的数按位与，只有公共前缀部分会保留
int rangeBitwiseAnd(int left, int right) {
    int shift = 0;
    while (left < right) {
        left >>= 1;
        right >>= 1;
        shift++;
    }
    return left << shift;
}

// ==================== 方法2: Brian Kernighan ====================
// 不断消除 right 最右边的 1
int rangeBitwiseAndBK(int left, int right) {
    while (left < right) {
        right &= (right - 1);
    }
    return right;
}

} // namespace Solution

// ==================== 测试函数 ====================
void runTests() {
    std::cout << "=== Bitwise And Range Solution ===" << std::endl;

    // 测试基本情况
    {
        // 5 = 101, 6 = 110, 7 = 111 -> 101 & 110 & 111 = 100 = 4
        assert(Solution::rangeBitwiseAnd(5, 7) == 4);
    }
    std::cout << "  Range Bitwise And (5,7): PASSED" << std::endl;

    // 测试相同数
    {
        assert(Solution::rangeBitwiseAnd(0, 0) == 0);
        assert(Solution::rangeBitwiseAnd(5, 5) == 5);
    }
    std::cout << "  Range Bitwise And (same): PASSED" << std::endl;

    // 测试大范围
    {
        assert(Solution::rangeBitwiseAnd(1, 2147483647) == 0);
    }
    std::cout << "  Range Bitwise And (large): PASSED" << std::endl;

    // 测试 Brian Kernighan 方法
    {
        assert(Solution::rangeBitwiseAndBK(5, 7) == 4);
        assert(Solution::rangeBitwiseAndBK(0, 0) == 0);
    }
    std::cout << "  Brian Kernighan method: PASSED" << std::endl;

    // 验证两种方法一致
    {
        assert(Solution::rangeBitwiseAnd(12, 15) == Solution::rangeBitwiseAndBK(12, 15));
    }
    std::cout << "  Both methods consistent: PASSED" << std::endl;
}

} // namespace BitwiseAndRangeImpl

/**
 * 关键要点：
 *
 * 1. 核心思想：找公共前缀
 *    - [left, right] 范围内所有数按位与
 *    - 结果是 left 和 right 的公共前缀
 *    - 后面的位一定会被清零
 *
 * 2. 方法1：移位法
 *    - 同时右移 left 和 right 直到相等
 *    - 记录移位次数
 *    - 结果左移回来
 *
 * 3. 方法2：Brian Kernighan
 *    - 不断消除 right 的最低位 1
 *    - 直到 right <= left
 *    - right 就是答案
 *
 * 4. 时间复杂度：O(log n)
 * 5. 空间复杂度：O(1)
 */
