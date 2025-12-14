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

// 题目1: 斐波那契数 - 迭代解法 O(n)
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

// 题目2: 爬楼梯
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

// 题目3: 使用最小花费爬楼梯
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

// 题目4: 第 N 个泰波那契数
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

// 题目5: 斐波那契数（矩阵快速幂）O(log n)
int fibMatrixPowSolution(int n) {
    if (n <= 1) return n;
    Matrix M = {{{1, 1}, {1, 0}}};
    Matrix result = matrixPow(M, n - 1);
    return static_cast<int>(result[0][0]);
}

// 题目6: 变态跳台阶 f(n) = 2^(n-1)
int jumpFloorIISolution(int n) {
    if (n <= 0) return 0;
    return 1 << (n - 1);
}

// 题目7: 矩形覆盖
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
