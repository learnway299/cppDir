/**
 * @file fibonacci_solution.cpp
 * @brief 斐波那契数列相关问题 - 参考答案
 */

#include "fibonacci.h"
#include <array>
#include <algorithm>
#include <iostream>
#include <cassert>

namespace Fibonacci {

// ==================== 内部辅助函数 ====================

namespace {
    constexpr int MOD = 1e9 + 7;
    using Matrix = std::array<std::array<long long, 2>, 2>;

    Matrix multiply(const Matrix& A, const Matrix& B) {
        Matrix C = {{{0, 0}, {0, 0}}};
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 2; ++k) {
                    C[i][j] = (C[i][j] + A[i][k] * B[k][j]) % MOD;
                }
            }
        }
        return C;
    }

    Matrix matrixPow(Matrix M, int n) {
        Matrix result = {{{1, 0}, {0, 1}}};
        while (n > 0) {
            if (n & 1) result = multiply(result, M);
            M = multiply(M, M);
            n >>= 1;
        }
        return result;
    }
}

// ==================== 参考答案实现 ====================

/**
 * 题目1: 斐波那契数 - 迭代法
 * 时间复杂度: O(n) - 从 2 迭代到 n
 * 空间复杂度: O(1) - 仅用两个变量滚动存储
 */
int fibSolution(int n) {
    if (n <= 1) return n;
    int prev2 = 0, prev1 = 1;
    for (int i = 2; i <= n; ++i) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

/**
 * 题目2: 爬楼梯 - 动态规划（斐波那契变形）
 * 时间复杂度: O(n) - 从 3 迭代到 n
 * 空间复杂度: O(1) - 仅用两个变量滚动存储
 */
int climbStairsSolution(int n) {
    if (n <= 2) return n;
    int prev2 = 1, prev1 = 2;
    for (int i = 3; i <= n; ++i) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

/**
 * 题目3: 使用最小花费爬楼梯 - 动态规划
 * 时间复杂度: O(n) - 遍历 cost 数组一次
 * 空间复杂度: O(1) - 仅用两个变量滚动存储，无需 dp 数组
 */
int minCostClimbingStairsSolution(std::vector<int>& cost) {
    int n = static_cast<int>(cost.size());
    int prev2 = 0, prev1 = 0;
    for (int i = 2; i <= n; ++i) {
        int curr = std::min(prev1 + cost[i - 1], prev2 + cost[i - 2]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

/**
 * 题目4: 第 N 个泰波那契数 - 迭代法
 * 时间复杂度: O(n) - 从 3 迭代到 n
 * 空间复杂度: O(1) - 仅用三个变量滚动存储
 */
int tribonacciSolution(int n) {
    if (n == 0) return 0;
    if (n <= 2) return 1;
    int t0 = 0, t1 = 1, t2 = 1;
    for (int i = 3; i <= n; ++i) {
        int t3 = t0 + t1 + t2;
        t0 = t1; t1 = t2; t2 = t3;
    }
    return t2;
}

/**
 * 题目5: 斐波那契数 - 矩阵快速幂
 * 时间复杂度: O(log n) - 快速幂将指数减半
 * 空间复杂度: O(1) - 固定大小 2x2 矩阵
 * 原理: [F(n), F(n-1)] = [1,1; 1,0]^(n-1) * [F(1), F(0)]
 */
int fibMatrixPowSolution(int n) {
    if (n <= 1) return n;
    Matrix M = {{{1, 1}, {1, 0}}};
    Matrix result = matrixPow(M, n - 1);
    return static_cast<int>(result[0][0]);
}

/**
 * 题目6: 变态跳台阶 - 数学公式 f(n) = 2^(n-1)
 * 时间复杂度: O(1) - 位运算直接计算
 * 空间复杂度: O(1) - 无额外空间
 * 推导: f(n) = f(n-1) + f(n-2) + ... + f(1) + 1，解得 f(n) = 2^(n-1)
 */
int jumpFloorIISolution(int n) {
    if (n <= 0) return 0;
    return 1 << (n - 1);
}

/**
 * 题目7: 矩形覆盖 - 动态规划（斐波那契变形）
 * 时间复杂度: O(n) - 从 3 迭代到 n
 * 空间复杂度: O(1) - 仅用两个变量滚动存储
 * 原理: 2xn 矩形可由 2x(n-1) 加竖条 或 2x(n-2) 加两横条得到
 */
int rectCoverSolution(int n) {
    if (n <= 2) return n;
    int prev2 = 1, prev1 = 2;
    for (int i = 3; i <= n; ++i) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

// ==================== 测试函数 ====================

void testFibonacciSolution() {
    std::cout << "=== Fibonacci Tests (Solution) ===\n";

    assert(fibSolution(0) == 0 && fibSolution(1) == 1 && fibSolution(10) == 55);
    std::cout << "  fibSolution: PASSED\n";

    assert(climbStairsSolution(2) == 2 && climbStairsSolution(5) == 8);
    std::cout << "  climbStairsSolution: PASSED\n";

    std::vector<int> cost1 = {10, 15, 20};
    assert(minCostClimbingStairsSolution(cost1) == 15);
    std::cout << "  minCostClimbingStairsSolution: PASSED\n";

    assert(tribonacciSolution(4) == 4 && tribonacciSolution(25) == 1389537);
    std::cout << "  tribonacciSolution: PASSED\n";

    assert(fibMatrixPowSolution(10) == 55);
    std::cout << "  fibMatrixPowSolution: PASSED\n";

    assert(jumpFloorIISolution(3) == 4 && jumpFloorIISolution(4) == 8);
    std::cout << "  jumpFloorIISolution: PASSED\n";

    assert(rectCoverSolution(4) == 5);
    std::cout << "  rectCoverSolution: PASSED\n";
}

} // namespace Fibonacci
