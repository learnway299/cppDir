/**
 * @file knapsack.h
 * @brief 背包问题 - 题目说明与声明
 *
 * 背包问题是 DP 的经典模型，包括 0-1 背包、完全背包、多重背包等变体。
 */

#ifndef KNAPSACK_H
#define KNAPSACK_H

#include <vector>
#include <string>

namespace Knapsack {

// ==================== 题目声明 ====================

/**
 * 题目1: 0-1 背包问题
 *
 * 有 n 件物品和一个最多能背重量为 W 的背包。
 * 每件物品只能用一次，求解将哪些物品装入背包里物品价值总和最大。
 */
int knapsack01(int W, std::vector<int>& weight, std::vector<int>& value);
int knapsack01Solution(int W, std::vector<int>& weight, std::vector<int>& value);

/**
 * 题目2: 分割等和子集
 *
 * 判断是否可以将数组分割成两个子集，使得两个子集的元素和相等。
 */
bool canPartition(std::vector<int>& nums);
bool canPartitionSolution(std::vector<int>& nums);

/**
 * 题目3: 目标和
 *
 * 向数组中的每个整数前添加 '+' 或 '-'，返回运算结果等于 target 的表达式数目。
 */
int findTargetSumWays(std::vector<int>& nums, int target);
int findTargetSumWaysSolution(std::vector<int>& nums, int target);

/**
 * 题目4: 最后一块石头的重量 II
 *
 * 返回最后剩下的石头的最小可能重量。
 */
int lastStoneWeightII(std::vector<int>& stones);
int lastStoneWeightIISolution(std::vector<int>& stones);

/**
 * 题目5: 完全背包 - 零钱兑换 II（组合数）
 *
 * 计算并返回可以凑成总金额的硬币组合数。
 */
int change(int amount, std::vector<int>& coins);
int changeSolution(int amount, std::vector<int>& coins);

/**
 * 题目6: 完全背包 - 组合总和 IV（排列数）
 *
 * 返回总和为 target 的元素组合的个数（顺序不同视为不同组合）。
 */
int combinationSum4(std::vector<int>& nums, int target);
int combinationSum4Solution(std::vector<int>& nums, int target);

/**
 * 题目7: 完全平方数
 *
 * 返回和为 n 的完全平方数的最少数量。
 */
int numSquares(int n);
int numSquaresSolution(int n);

/**
 * 题目8: 单词拆分
 *
 * 判断是否可以利用字典中出现的单词拼接出 s。
 */
bool wordBreak(std::string s, std::vector<std::string>& wordDict);
bool wordBreakSolution(std::string s, std::vector<std::string>& wordDict);

/**
 * 题目9: 一和零
 *
 * 找出并返回最大子集的长度，该子集中最多有 m 个 0 和 n 个 1。
 */
int findMaxForm(std::vector<std::string>& strs, int m, int n);
int findMaxFormSolution(std::vector<std::string>& strs, int m, int n);

// ==================== 测试函数声明 ====================

void testKnapsack();          // 测试面试者实现
void testKnapsackSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testKnapsack();
    testKnapsackSolution();
}

} // namespace Knapsack

#endif // KNAPSACK_H
