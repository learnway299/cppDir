/**
 * @file fibonacci.cpp
 * @brief 斐波那契数列相关问题 - 题目文件
 *
 * 斐波那契数列是面试中的经典问题，涉及递归、动态规划、矩阵快速幂等多种技巧。
 * 每个函数需要你实现具体逻辑。
 */

#include <vector>
#include <cstdint>

/**
 * 题目1: 斐波那契数
 *
 * 斐波那契数列定义如下：
 *   F(0) = 0, F(1) = 1
 *   F(n) = F(n-1) + F(n-2), n > 1
 *
 * 给定 n，计算 F(n)。
 *
 * 示例:
 *   输入: n = 2   输出: 1  (F(2) = F(1) + F(0) = 1 + 0 = 1)
 *   输入: n = 3   输出: 2  (F(3) = F(2) + F(1) = 1 + 1 = 2)
 *   输入: n = 4   输出: 3  (F(4) = F(3) + F(2) = 2 + 1 = 3)
 *   输入: n = 10  输出: 55
 *
 * 要求: 实现时间复杂度 O(n) 的解法
 *
 * @param n 第 n 个斐波那契数
 * @return F(n) 的值
 */
int fib(int n) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目2: 爬楼梯
 *
 * 假设你正在爬楼梯，需要 n 阶才能到达楼顶。
 * 每次你可以爬 1 或 2 个台阶。
 * 你有多少种不同的方法可以爬到楼顶？
 *
 * 示例:
 *   输入: n = 2  输出: 2
 *   解释: 有两种方法：1+1, 2
 *
 *   输入: n = 3  输出: 3
 *   解释: 有三种方法：1+1+1, 1+2, 2+1
 *
 *   输入: n = 4  输出: 5
 *   解释: 有五种方法：1+1+1+1, 1+1+2, 1+2+1, 2+1+1, 2+2
 *
 * @param n 楼梯阶数
 * @return 爬到楼顶的方法数
 */
int climbStairs(int n) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目3: 使用最小花费爬楼梯
 *
 * 给你一个整数数组 cost，其中 cost[i] 是从楼梯第 i 个台阶向上爬需要支付的费用。
 * 一旦支付此费用，即可选择向上爬 1 或 2 个台阶。
 * 你可以选择从下标为 0 或下标为 1 的台阶开始爬楼梯。
 * 请返回达到楼梯顶部的最低花费。
 *
 * 示例:
 *   输入: cost = [10,15,20]
 *   输出: 15
 *   解释: 从 cost[1] 开始，支付 15，爬两个台阶到达顶部
 *
 *   输入: cost = [1,100,1,1,1,100,1,1,100,1]
 *   输出: 6
 *   解释: 从 cost[0] 开始，选择最优路径
 *
 * @param cost 每个台阶的花费
 * @return 最小花费
 */
int minCostClimbingStairs(std::vector<int>& cost) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目4: 第 N 个泰波那契数
 *
 * 泰波那契序列 Tn 定义如下：
 *   T0 = 0, T1 = 1, T2 = 1
 *   Tn = Tn-1 + Tn-2 + Tn-3, n >= 3
 *
 * 给定 n，返回第 n 个泰波那契数 Tn。
 *
 * 示例:
 *   输入: n = 4   输出: 4
 *   解释: T4 = T3 + T2 + T1 = 2 + 1 + 1 = 4
 *
 *   输入: n = 25  输出: 1389537
 *
 * @param n 第 n 个泰波那契数
 * @return Tn 的值
 */
int tribonacci(int n) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目5: 斐波那契数（大数版本）
 *
 * 计算第 n 个斐波那契数，结果对 10^9 + 7 取模。
 * 要求: 时间复杂度 O(log n)，使用矩阵快速幂。
 *
 * 矩阵快速幂原理:
 *   [F(n+1)]   [1 1]^n   [F(1)]
 *   [F(n)  ] = [1 0]   * [F(0)]
 *
 * 示例:
 *   输入: n = 50   输出: 12586269025 % (10^9+7) = 586268941
 *
 * @param n 第 n 个斐波那契数
 * @return F(n) % (10^9 + 7)
 */
int fibMatrixPow(int n) {
    // TODO: 实现你的解法（矩阵快速幂）
    return 0;
}

/**
 * 题目6: 变态跳台阶
 *
 * 一只青蛙一次可以跳上 1 级台阶，也可以跳上 2 级... 它也可以跳上 n 级。
 * 求该青蛙跳上一个 n 级的台阶总共有多少种跳法。
 *
 * 数学推导:
 *   f(n) = f(n-1) + f(n-2) + ... + f(1) + f(0)
 *   f(n-1) = f(n-2) + f(n-3) + ... + f(1) + f(0)
 *   因此: f(n) = 2 * f(n-1)
 *   所以: f(n) = 2^(n-1), n >= 1
 *
 * 示例:
 *   输入: n = 1  输出: 1
 *   输入: n = 2  输出: 2
 *   输入: n = 3  输出: 4
 *   输入: n = 4  输出: 8
 *
 * @param n 台阶数
 * @return 跳法总数
 */
int jumpFloorII(int n) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目7: 矩形覆盖
 *
 * 我们可以用 2*1 的小矩形横着或者竖着去覆盖更大的矩形。
 * 请问用 n 个 2*1 的小矩形无重叠地覆盖一个 2*n 的大矩形，总共有多少种方法？
 *
 * 分析:
 *   - 放第一块时，如果竖着放，剩余 2*(n-1) 需要覆盖
 *   - 如果横着放，必须放两块，剩余 2*(n-2) 需要覆盖
 *   - f(n) = f(n-1) + f(n-2)，即斐波那契数列
 *
 * 示例:
 *   输入: n = 1  输出: 1  (只能竖着放)
 *   输入: n = 2  输出: 2  (两个竖着或两个横着)
 *   输入: n = 3  输出: 3
 *   输入: n = 4  输出: 5
 *
 * @param n 大矩形的宽度
 * @return 覆盖方法数
 */
int rectCover(int n) {
    // TODO: 实现你的解法
    return 0;
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
    std::cout << "fib: PASSED\n";
}

void testClimbStairs() {
    assert(climbStairs(1) == 1);
    assert(climbStairs(2) == 2);
    assert(climbStairs(3) == 3);
    assert(climbStairs(4) == 5);
    assert(climbStairs(5) == 8);
    std::cout << "climbStairs: PASSED\n";
}

void testMinCostClimbingStairs() {
    std::vector<int> cost1 = {10, 15, 20};
    assert(minCostClimbingStairs(cost1) == 15);

    std::vector<int> cost2 = {1, 100, 1, 1, 1, 100, 1, 1, 100, 1};
    assert(minCostClimbingStairs(cost2) == 6);
    std::cout << "minCostClimbingStairs: PASSED\n";
}

void testTribonacci() {
    assert(tribonacci(0) == 0);
    assert(tribonacci(1) == 1);
    assert(tribonacci(2) == 1);
    assert(tribonacci(4) == 4);
    assert(tribonacci(25) == 1389537);
    std::cout << "tribonacci: PASSED\n";
}

void testFibMatrixPow() {
    assert(fibMatrixPow(0) == 0);
    assert(fibMatrixPow(1) == 1);
    assert(fibMatrixPow(10) == 55);
    // fib(50) = 12586269025, mod 10^9+7 = 586268941
    // assert(fibMatrixPow(50) == 586268941);
    std::cout << "fibMatrixPow: PASSED\n";
}

void testJumpFloorII() {
    assert(jumpFloorII(1) == 1);
    assert(jumpFloorII(2) == 2);
    assert(jumpFloorII(3) == 4);
    assert(jumpFloorII(4) == 8);
    std::cout << "jumpFloorII: PASSED\n";
}

void testRectCover() {
    assert(rectCover(1) == 1);
    assert(rectCover(2) == 2);
    assert(rectCover(3) == 3);
    assert(rectCover(4) == 5);
    std::cout << "rectCover: PASSED\n";
}

int main() {
    std::cout << "=== Fibonacci Problems Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testFib();
    // testClimbStairs();
    // testMinCostClimbingStairs();
    // testTribonacci();
    // testFibMatrixPow();
    // testJumpFloorII();
    // testRectCover();

    std::cout << "All tests passed!\n";
    return 0;
}
