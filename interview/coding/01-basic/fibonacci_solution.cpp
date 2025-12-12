/**
 * @file fibonacci_solution.cpp
 * @brief 斐波那契数列相关问题 - 解答文件
 *
 * 包含 fibonacci.cpp 中所有题目的解答和详细解析。
 * 涵盖递归、迭代、动态规划、矩阵快速幂等多种解法。
 */

#include <vector>
#include <array>
#include <cstdint>

/**
 * 题目1: 斐波那契数
 *
 * 解法1: 迭代（推荐）
 * 思路: 用两个变量滚动计算，空间 O(1)
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int fib(int n) {
    if (n <= 1) return n;

    int prev2 = 0;  // F(n-2)
    int prev1 = 1;  // F(n-1)

    for (int i = 2; i <= n; ++i) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }

    return prev1;
}

// 解法2: 递归（不推荐，仅作对比）
// 时间复杂度: O(2^n) - 指数级，会超时
int fibRecursive(int n) {
    if (n <= 1) return n;
    return fibRecursive(n - 1) + fibRecursive(n - 2);
}

// 解法3: 带记忆化的递归
// 时间复杂度: O(n), 空间复杂度: O(n)
int fibMemo(int n, std::vector<int>& memo) {
    if (n <= 1) return n;
    if (memo[n] != -1) return memo[n];
    memo[n] = fibMemo(n - 1, memo) + fibMemo(n - 2, memo);
    return memo[n];
}

int fibMemoized(int n) {
    std::vector<int> memo(n + 1, -1);
    return fibMemo(n, memo);
}

// 解法4: 动态规划数组版本
// 时间复杂度: O(n), 空间复杂度: O(n)
int fibDP(int n) {
    if (n <= 1) return n;

    std::vector<int> dp(n + 1);
    dp[0] = 0;
    dp[1] = 1;

    for (int i = 2; i <= n; ++i) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }

    return dp[n];
}

/**
 * 题目2: 爬楼梯
 *
 * 解法: 动态规划
 * 思路:
 *   - 到达第 n 阶的方法 = 到达第 n-1 阶的方法 + 到达第 n-2 阶的方法
 *   - dp[n] = dp[n-1] + dp[n-2]
 *   - 本质上就是斐波那契数列，只是初始值不同
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int climbStairs(int n) {
    if (n <= 2) return n;

    int prev2 = 1;  // 到达第 1 阶的方法数
    int prev1 = 2;  // 到达第 2 阶的方法数

    for (int i = 3; i <= n; ++i) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }

    return prev1;
}

/**
 * 题目3: 使用最小花费爬楼梯
 *
 * 解法: 动态规划
 * 思路:
 *   - dp[i] = 到达第 i 阶的最小花费
 *   - dp[i] = min(dp[i-1] + cost[i-1], dp[i-2] + cost[i-2])
 *   - 可以从 i-1 跳一步或从 i-2 跳两步到达 i
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int minCostClimbingStairs(std::vector<int>& cost) {
    int n = static_cast<int>(cost.size());

    // dp[i] 表示到达第 i 个台阶顶部的最小花费
    int prev2 = 0;  // 站在起点，花费 0
    int prev1 = 0;  // 站在起点，花费 0

    for (int i = 2; i <= n; ++i) {
        int curr = std::min(prev1 + cost[i - 1], prev2 + cost[i - 2]);
        prev2 = prev1;
        prev1 = curr;
    }

    return prev1;
}

// 另一种理解方式: dp[i] = 从第 i 个台阶出发到顶部的最小花费
int minCostClimbingStairsV2(std::vector<int>& cost) {
    int n = static_cast<int>(cost.size());
    std::vector<int> dp(n + 1, 0);

    // 从后往前计算
    for (int i = n - 1; i >= 0; --i) {
        if (i == n - 1) {
            dp[i] = cost[i];  // 最后一阶，只需支付当前费用
        } else if (i == n - 2) {
            dp[i] = cost[i];  // 倒数第二阶，可以直接跳到顶部
        } else {
            dp[i] = cost[i] + std::min(dp[i + 1], dp[i + 2]);
        }
    }

    return std::min(dp[0], dp[1]);  // 从 0 或 1 开始
}

/**
 * 题目4: 第 N 个泰波那契数
 *
 * 解法: 滚动数组
 * 思路: 维护三个变量，滚动计算
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int tribonacci(int n) {
    if (n == 0) return 0;
    if (n <= 2) return 1;

    int t0 = 0, t1 = 1, t2 = 1;

    for (int i = 3; i <= n; ++i) {
        int t3 = t0 + t1 + t2;
        t0 = t1;
        t1 = t2;
        t2 = t3;
    }

    return t2;
}

/**
 * 题目5: 斐波那契数（矩阵快速幂）
 *
 * 解法: 矩阵快速幂
 * 原理:
 *   [F(n+1)]   [1 1]^n   [F(1)]   [1 1]^n   [1]
 *   [F(n)  ] = [1 0]   * [F(0)] = [1 0]   * [0]
 *
 * 时间复杂度: O(log n)
 * 空间复杂度: O(1)
 */
const int MOD = 1e9 + 7;

using Matrix = std::array<std::array<long long, 2>, 2>;

// 矩阵乘法
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

// 矩阵快速幂
Matrix matrixPow(Matrix M, int n) {
    Matrix result = {{{1, 0}, {0, 1}}};  // 单位矩阵

    while (n > 0) {
        if (n & 1) {
            result = multiply(result, M);
        }
        M = multiply(M, M);
        n >>= 1;
    }

    return result;
}

int fibMatrixPow(int n) {
    if (n <= 1) return n;

    Matrix M = {{{1, 1}, {1, 0}}};
    Matrix result = matrixPow(M, n - 1);

    // result[0][0] = F(n), result[0][1] = F(n-1)
    return static_cast<int>(result[0][0]);
}

/**
 * 题目6: 变态跳台阶
 *
 * 解法: 数学公式
 * 推导:
 *   f(n) = f(n-1) + f(n-2) + ... + f(1) + f(0)  ... (1)
 *   f(n-1) = f(n-2) + f(n-3) + ... + f(1) + f(0)  ... (2)
 *   (1) - (2): f(n) - f(n-1) = f(n-1)
 *   所以: f(n) = 2 * f(n-1) = 2^(n-1)
 *
 * 时间复杂度: O(1) 或 O(log n)（取决于幂运算实现）
 * 空间复杂度: O(1)
 */
int jumpFloorII(int n) {
    if (n <= 0) return 0;
    // f(n) = 2^(n-1)
    return 1 << (n - 1);  // 位运算实现 2^(n-1)
}

// 动态规划解法（验证用）
int jumpFloorIIDP(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;

    std::vector<int> dp(n + 1);
    dp[0] = 1;  // 站在起点也算一种
    dp[1] = 1;

    for (int i = 2; i <= n; ++i) {
        dp[i] = 0;
        for (int j = 0; j < i; ++j) {
            dp[i] += dp[j];  // 从第 j 阶跳到第 i 阶
        }
    }

    return dp[n];
}

/**
 * 题目7: 矩形覆盖
 *
 * 解法: 斐波那契数列
 * 分析:
 *   - 第一块竖着放: 剩下 f(n-1) 种方法
 *   - 第一块横着放（必须放两块）: 剩下 f(n-2) 种方法
 *   - f(n) = f(n-1) + f(n-2)
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int rectCover(int n) {
    if (n <= 2) return n;

    int prev2 = 1;  // f(1)
    int prev1 = 2;  // f(2)

    for (int i = 3; i <= n; ++i) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }

    return prev1;
}


// ==================== 扩展: 斐波那契数列的其他性质 ====================

/**
 * 斐波那契数列的通项公式（Binet 公式）
 * F(n) = (φ^n - ψ^n) / √5
 * 其中 φ = (1 + √5) / 2 ≈ 1.618 (黄金比例)
 *      ψ = (1 - √5) / 2 ≈ -0.618
 *
 * 注意: 由于浮点精度问题，n 较大时不精确
 */
#include <cmath>

int fibBinet(int n) {
    double sqrt5 = std::sqrt(5);
    double phi = (1 + sqrt5) / 2;
    double psi = (1 - sqrt5) / 2;
    return static_cast<int>(std::round((std::pow(phi, n) - std::pow(psi, n)) / sqrt5));
}

/**
 * 最大公约数与斐波那契数列的关系
 * gcd(F(m), F(n)) = F(gcd(m, n))
 */

/**
 * 斐波那契数列的和
 * F(1) + F(2) + ... + F(n) = F(n+2) - 1
 */
long long fibSum(int n) {
    if (n <= 0) return 0;
    // 计算 F(n+2) - 1
    return fib(n + 2) - 1;
}


// ==================== 测试代码 ====================
#include <iostream>
#include <cassert>

void testFib() {
    assert(fib(0) == 0);
    assert(fib(1) == 1);
    assert(fib(2) == 1);
    assert(fib(3) == 2);
    assert(fib(4) == 3);
    assert(fib(10) == 55);
    assert(fib(20) == 6765);
    std::cout << "fib: PASSED\n";
}

void testClimbStairs() {
    assert(climbStairs(1) == 1);
    assert(climbStairs(2) == 2);
    assert(climbStairs(3) == 3);
    assert(climbStairs(4) == 5);
    assert(climbStairs(5) == 8);
    assert(climbStairs(10) == 89);
    std::cout << "climbStairs: PASSED\n";
}

void testMinCostClimbingStairs() {
    std::vector<int> cost1 = {10, 15, 20};
    assert(minCostClimbingStairs(cost1) == 15);

    std::vector<int> cost2 = {1, 100, 1, 1, 1, 100, 1, 1, 100, 1};
    assert(minCostClimbingStairs(cost2) == 6);

    std::vector<int> cost3 = {0, 0, 0, 1};
    assert(minCostClimbingStairs(cost3) == 0);
    std::cout << "minCostClimbingStairs: PASSED\n";
}

void testTribonacci() {
    assert(tribonacci(0) == 0);
    assert(tribonacci(1) == 1);
    assert(tribonacci(2) == 1);
    assert(tribonacci(3) == 2);
    assert(tribonacci(4) == 4);
    assert(tribonacci(25) == 1389537);
    std::cout << "tribonacci: PASSED\n";
}

void testFibMatrixPow() {
    assert(fibMatrixPow(0) == 0);
    assert(fibMatrixPow(1) == 1);
    assert(fibMatrixPow(2) == 1);
    assert(fibMatrixPow(10) == 55);
    assert(fibMatrixPow(20) == 6765);
    std::cout << "fibMatrixPow: PASSED\n";
}

void testJumpFloorII() {
    assert(jumpFloorII(1) == 1);
    assert(jumpFloorII(2) == 2);
    assert(jumpFloorII(3) == 4);
    assert(jumpFloorII(4) == 8);
    assert(jumpFloorII(5) == 16);
    assert(jumpFloorII(10) == 512);
    std::cout << "jumpFloorII: PASSED\n";
}

void testRectCover() {
    assert(rectCover(1) == 1);
    assert(rectCover(2) == 2);
    assert(rectCover(3) == 3);
    assert(rectCover(4) == 5);
    assert(rectCover(5) == 8);
    assert(rectCover(10) == 89);
    std::cout << "rectCover: PASSED\n";
}

void testBinet() {
    // Binet 公式验证（小 n 值）
    for (int i = 0; i <= 20; ++i) {
        assert(fibBinet(i) == fib(i));
    }
    std::cout << "fibBinet: PASSED\n";
}

int main() {
    std::cout << "=== Fibonacci Solutions Tests ===\n";

    testFib();
    testClimbStairs();
    testMinCostClimbingStairs();
    testTribonacci();
    testFibMatrixPow();
    testJumpFloorII();
    testRectCover();
    testBinet();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
