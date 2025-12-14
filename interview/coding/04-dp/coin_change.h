/**
 * @file coin_change.h
 * @brief 零钱兑换问题 - 题目说明与声明
 *
 * 零钱兑换是完全背包的经典应用，有多种变体。
 */

#ifndef COIN_CHANGE_H
#define COIN_CHANGE_H

#include <vector>

namespace CoinChange {

// ==================== 题目声明 ====================

/**
 * 题目1: 零钱兑换（最少硬币数）
 *
 * 每种硬币数量无限，返回凑成 amount 所需的最少硬币数。
 * 状态转移: dp[j] = min(dp[j], dp[j - coin] + 1)
 */
int coinChange(std::vector<int>& coins, int amount);
int coinChangeSolution(std::vector<int>& coins, int amount);

/**
 * 题目2: 零钱兑换 II（组合数）
 *
 * 返回凑成 amount 的组合数（无序）。
 */
int change(int amount, std::vector<int>& coins);
int changeSolution(int amount, std::vector<int>& coins);

/**
 * 题目3: 组合总和 IV（排列数）
 *
 * 顺序不同的序列被视作不同的组合（有序）。
 */
int combinationSum4(std::vector<int>& nums, int target);
int combinationSum4Solution(std::vector<int>& nums, int target);

/**
 * 题目4: 爬楼梯（扩展版）
 *
 * 给定每次可爬的台阶数选项，返回爬到 n 阶的方法数。
 */
int climbStairsExtended(int n, std::vector<int>& steps);
int climbStairsExtendedSolution(int n, std::vector<int>& steps);

/**
 * 题目5: 最大硬币价值
 *
 * 选择最多 maxCoins 个硬币，使总金额最接近但不超过 amount。
 */
int maxCoinValue(std::vector<int>& coins, int maxCoins, int amount);
int maxCoinValueSolution(std::vector<int>& coins, int maxCoins, int amount);

/**
 * 题目6: 凑零钱的方案（打印所有组合）
 *
 * 返回所有可以凑成总金额的硬币组合。
 */
std::vector<std::vector<int>> coinChangeCombinations(std::vector<int>& coins, int amount);
std::vector<std::vector<int>> coinChangeCombinationsSolution(std::vector<int>& coins, int amount);

/**
 * 题目7: 最少硬币数（有限数量）
 *
 * 多重背包问题：coins[i] = [value, count]
 */
int coinChangeLimited(std::vector<std::vector<int>>& coins, int amount);
int coinChangeLimitedSolution(std::vector<std::vector<int>>& coins, int amount);

// ==================== 测试函数声明 ====================

void testCoinChange();          // 测试面试者实现
void testCoinChangeSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testCoinChange();
    testCoinChangeSolution();
}

} // namespace CoinChange

#endif // COIN_CHANGE_H
