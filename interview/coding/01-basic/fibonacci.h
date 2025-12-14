/**
 * @file fibonacci.h
 * @brief 斐波那契数列相关问题 - 题目说明与声明
 *
 * 斐波那契数列是面试中的经典问题，涉及递归、动态规划、矩阵快速幂等多种技巧。
 */

#ifndef FIBONACCI_H
#define FIBONACCI_H

#include <vector>

namespace Fibonacci {

// ==================== 题目声明 ====================

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
 *   输入: n = 10  输出: 55
 *
 * 要求: 实现时间复杂度 O(n) 的解法
 */
int fib(int n);
int fibSolution(int n);

/**
 * 题目2: 爬楼梯
 *
 * 假设你正在爬楼梯，需要 n 阶才能到达楼顶。
 * 每次你可以爬 1 或 2 个台阶。
 * 你有多少种不同的方法可以爬到楼顶？
 *
 * 示例:
 *   输入: n = 2  输出: 2  (1+1, 2)
 *   输入: n = 3  输出: 3  (1+1+1, 1+2, 2+1)
 *   输入: n = 4  输出: 5
 */
int climbStairs(int n);
int climbStairsSolution(int n);

/**
 * 题目3: 使用最小花费爬楼梯
 *
 * 给你一个整数数组 cost，其中 cost[i] 是从楼梯第 i 个台阶向上爬需要支付的费用。
 * 一旦支付此费用，即可选择向上爬 1 或 2 个台阶。
 * 你可以选择从下标为 0 或下标为 1 的台阶开始爬楼梯。
 * 请返回达到楼梯顶部的最低花费。
 *
 * 示例:
 *   输入: cost = [10,15,20]  输出: 15
 *   输入: cost = [1,100,1,1,1,100,1,1,100,1]  输出: 6
 */
int minCostClimbingStairs(std::vector<int>& cost);
int minCostClimbingStairsSolution(std::vector<int>& cost);

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
 *   输入: n = 25  输出: 1389537
 */
int tribonacci(int n);
int tribonacciSolution(int n);

/**
 * 题目5: 斐波那契数（大数版本）
 *
 * 计算第 n 个斐波那契数，结果对 10^9 + 7 取模。
 * 要求: 时间复杂度 O(log n)，使用矩阵快速幂。
 *
 * 矩阵快速幂原理:
 *   [F(n+1)]   [1 1]^n   [F(1)]
 *   [F(n)  ] = [1 0]   * [F(0)]
 */
int fibMatrixPow(int n);
int fibMatrixPowSolution(int n);

/**
 * 题目6: 变态跳台阶
 *
 * 一只青蛙一次可以跳上 1 级台阶，也可以跳上 2 级... 它也可以跳上 n 级。
 * 求该青蛙跳上一个 n 级的台阶总共有多少种跳法。
 *
 * 数学推导: f(n) = 2^(n-1), n >= 1
 *
 * 示例:
 *   输入: n = 1  输出: 1
 *   输入: n = 3  输出: 4
 */
int jumpFloorII(int n);
int jumpFloorIISolution(int n);

/**
 * 题目7: 矩形覆盖
 *
 * 我们可以用 2*1 的小矩形横着或者竖着去覆盖更大的矩形。
 * 请问用 n 个 2*1 的小矩形无重叠地覆盖一个 2*n 的大矩形，总共有多少种方法？
 *
 * 分析: f(n) = f(n-1) + f(n-2)，即斐波那契数列
 *
 * 示例:
 *   输入: n = 1  输出: 1
 *   输入: n = 4  输出: 5
 */
int rectCover(int n);
int rectCoverSolution(int n);

// ==================== 测试函数声明 ====================

void testFibonacci();          // 测试面试者实现
void testFibonacciSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testFibonacci();
    testFibonacciSolution();
}

} // namespace Fibonacci

#endif // FIBONACCI_H
