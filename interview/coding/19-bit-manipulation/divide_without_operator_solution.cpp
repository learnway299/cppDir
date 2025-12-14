/**
 * @file divide_without_operator_solution.cpp
 * @brief 不用除法实现除法 - 参考解答
 */

#include "divide_without_operator.h"
#include <iostream>
#include <climits>
#include <cassert>

namespace DivideWithoutOperatorImpl {
namespace Solution {

// ==================== 位运算实现除法 ====================
// 通过倍增法，每次尝试减去 divisor * 2^k
int divide(int dividend, int divisor) {
    // 处理溢出情况
    if (dividend == INT_MIN && divisor == -1) return INT_MAX;
    if (dividend == INT_MIN && divisor == 1) return INT_MIN;

    // 记录结果符号
    bool negative = (dividend < 0) ^ (divisor < 0);

    // 转为负数处理，避免INT_MIN转正数溢出
    long a = dividend < 0 ? dividend : -(long)dividend;
    long b = divisor < 0 ? divisor : -(long)divisor;

    int result = 0;
    while (a <= b) {  // 负数比较，a <= b 表示 |a| >= |b|
        long temp = b;
        int count = 1;
        // 倍增找到最大的 2^k 使得 temp * 2^k <= a (负数)
        while (a <= temp + temp && temp + temp < 0) {
            temp += temp;
            count += count;
        }
        a -= temp;
        result += count;
    }

    return negative ? -result : result;
}

} // namespace Solution

// ==================== 测试函数 ====================
void runTests() {
    std::cout << "=== Divide Without Operator Solution ===" << std::endl;

    // 测试正数除法
    {
        assert(Solution::divide(10, 3) == 3);
    }
    std::cout << "  Divide (10/3): PASSED" << std::endl;

    // 测试负数除法
    {
        assert(Solution::divide(7, -3) == -2);
        assert(Solution::divide(-7, 3) == -2);
        assert(Solution::divide(-7, -3) == 2);
    }
    std::cout << "  Divide (negative): PASSED" << std::endl;

    // 测试边界情况
    {
        assert(Solution::divide(INT_MIN, -1) == INT_MAX);  // 溢出处理
        assert(Solution::divide(1, 1) == 1);
        assert(Solution::divide(0, 1) == 0);
    }
    std::cout << "  Divide (edge cases): PASSED" << std::endl;

    // 测试除数为1或-1
    {
        assert(Solution::divide(100, 1) == 100);
        assert(Solution::divide(100, -1) == -100);
    }
    std::cout << "  Divide by 1: PASSED" << std::endl;
}

} // namespace DivideWithoutOperatorImpl

/**
 * 关键要点：
 *
 * 1. 倍增法思想：
 *    - 不能用乘除，但可以用位移（相当于乘2）
 *    - 每次找最大的 2^k 使得 divisor * 2^k <= dividend
 *    - 减去后继续处理剩余部分
 *
 * 2. 处理负数：
 *    - 转为负数处理，避免 INT_MIN 转正数溢出
 *    - 负数比较逻辑相反
 *
 * 3. 边界情况：
 *    - INT_MIN / -1 会溢出，返回 INT_MAX
 *    - INT_MIN / 1 返回 INT_MIN
 *
 * 4. 符号处理：
 *    - 异或判断结果符号
 *    - (a < 0) ^ (b < 0) 表示异号
 *
 * 5. 时间复杂度：O(log^2 n)
 * 6. 空间复杂度：O(1)
 */
