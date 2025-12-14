/**
 * @file power_of_two_solution.cpp
 * @brief 2的幂 - 参考解答
 */

#include "power_of_two.h"
#include <iostream>
#include <cassert>

namespace PowerOfTwoImpl {
namespace Solution {

// ==================== 题目1: 2的幂 ====================
bool isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// ==================== 题目2: 4的幂 ====================
// 0x55555555 = 0101 0101 ... (奇数位为1)
bool isPowerOfFour(int n) {
    return n > 0 && (n & (n - 1)) == 0 && (n & 0x55555555);
}

} // namespace Solution

// ==================== 测试函数 ====================
void runTests() {
    std::cout << "=== Power of Two Solution ===" << std::endl;

    // 测试2的幂
    {
        assert(Solution::isPowerOfTwo(1) == true);   // 2^0
        assert(Solution::isPowerOfTwo(16) == true);  // 2^4
        assert(Solution::isPowerOfTwo(3) == false);
        assert(Solution::isPowerOfTwo(0) == false);
    }
    std::cout << "  Is Power of Two: PASSED" << std::endl;

    // 测试4的幂
    {
        assert(Solution::isPowerOfFour(1) == true);   // 4^0
        assert(Solution::isPowerOfFour(16) == true);  // 4^2
        assert(Solution::isPowerOfFour(8) == false);  // 2^3，不是4的幂
        assert(Solution::isPowerOfFour(5) == false);
    }
    std::cout << "  Is Power of Four: PASSED" << std::endl;
}

} // namespace PowerOfTwoImpl

/**
 * 关键要点：
 *
 * 1. 2的幂判断：
 *    - n > 0 且 n & (n-1) == 0
 *    - 2的幂的二进制只有一个 1
 *    - n & (n-1) 消除最低位的 1
 *
 * 2. 4的幂判断：
 *    - 首先是2的幂
 *    - 且 1 在奇数位上（从右往左第 0, 2, 4... 位）
 *    - 0x55555555 = 01010101... 用于检测
 *
 * 3. 其他方法：
 *    - 4的幂：n % 3 == 1
 *    - 原因：4^k = (3+1)^k ≡ 1 (mod 3)
 *
 * 4. 时间复杂度：O(1)
 * 5. 空间复杂度：O(1)
 */
