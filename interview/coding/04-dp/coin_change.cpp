/**
 * @file coin_change.cpp
 * @brief 零钱兑换问题 - 题目文件
 *
 * 零钱兑换是完全背包的经典应用，有多种变体。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

/**
 * 题目1: 零钱兑换（最少硬币数）
 *
 * 给你一个整数数组 coins，表示不同面额的硬币；
 * 以及一个整数 amount，表示总金额。
 *
 * 计算并返回可以凑成总金额所需的最少的硬币个数。
 * 如果没有任何一种硬币组合能组成总金额，返回 -1。
 *
 * 你可以认为每种硬币的数量是无限的。
 *
 * 示例:
 *   输入: coins = [1,2,5], amount = 11
 *   输出: 3
 *   解释: 11 = 5 + 5 + 1
 *
 *   输入: coins = [2], amount = 3
 *   输出: -1
 *
 *   输入: coins = [1], amount = 0
 *   输出: 0
 *
 * 状态定义: dp[j] = 凑成金额 j 所需的最少硬币数
 * 状态转移: dp[j] = min(dp[j], dp[j - coin] + 1)
 *
 * @param coins 硬币面额数组
 * @param amount 目标金额
 * @return 最少硬币数，无解返回 -1
 */
int coinChange(std::vector<int>& coins, int amount) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目2: 零钱兑换 II（组合数）
 *
 * 给你一个整数数组 coins 表示不同面额的硬币，另给一个整数 amount 表示总金额。
 * 请你计算并返回可以凑成总金额的硬币组合数。如果任何硬币组合都无法凑出总金额，返回 0。
 *
 * 假设每一种面额的硬币有无限个。
 *
 * 示例:
 *   输入: amount = 5, coins = [1,2,5]
 *   输出: 4
 *   解释: 有四种方式可以凑成总金额：
 *         5=5
 *         5=2+2+1
 *         5=2+1+1+1
 *         5=1+1+1+1+1
 *
 *   输入: amount = 3, coins = [2]
 *   输出: 0
 *
 *   输入: amount = 10, coins = [10]
 *   输出: 1
 *
 * 注意：这是组合问题，不是排列问题
 *
 * @param amount 目标金额
 * @param coins 硬币面额数组
 * @return 组合数
 */
int change(int amount, std::vector<int>& coins) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目3: 组合总和 IV（排列数）
 *
 * 给你一个由不同整数组成的数组 nums，和一个目标整数 target。
 * 请你从 nums 中找出并返回总和为 target 的元素组合的个数。
 *
 * 请注意，顺序不同的序列被视作不同的组合。
 *
 * 示例:
 *   输入: nums = [1,2,3], target = 4
 *   输出: 7
 *   解释: 所有可能的组合为：
 *         (1,1,1,1)
 *         (1,1,2)
 *         (1,2,1)
 *         (2,1,1)
 *         (2,2)
 *         (1,3)
 *         (3,1)
 *
 * 与零钱兑换 II 的区别：这是排列问题（有序），遍历顺序不同
 *
 * @param nums 整数数组
 * @param target 目标和
 * @return 排列数
 */
int combinationSum4(std::vector<int>& nums, int target) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目4: 爬楼梯（扩展版）
 *
 * 假设你正在爬楼梯。需要 n 阶你才能到达楼顶。
 * 给定一个数组 steps，表示每次可以爬的台阶数选项。
 * 你有多少种不同的方法可以爬到楼顶呢？
 *
 * 示例:
 *   输入: n = 5, steps = [1,2]
 *   输出: 8
 *   解释: 等同于普通爬楼梯问题
 *
 *   输入: n = 5, steps = [1,2,5]
 *   输出: 9
 *
 * @param n 楼梯阶数
 * @param steps 每次可爬的台阶数
 * @return 方法数
 */
int climbStairsExtended(int n, std::vector<int>& steps) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目5: 最大硬币价值
 *
 * 给你一个整数数组 coins，表示不同面额的硬币；
 * 以及两个整数 maxCoins 和 amount。
 *
 * 选择最多 maxCoins 个硬币，使得总金额最接近但不超过 amount。
 * 返回能凑成的最大金额。
 *
 * 示例:
 *   输入: coins = [1,2,5], maxCoins = 3, amount = 11
 *   输出: 11
 *   解释: 选择 5+5+1=11
 *
 *   输入: coins = [3,7], maxCoins = 2, amount = 10
 *   输出: 10
 *   解释: 选择 3+7=10
 *
 * 思路: 二维 DP，增加硬币数量维度
 *
 * @param coins 硬币面额数组
 * @param maxCoins 最多使用的硬币数
 * @param amount 目标金额上限
 * @return 最大金额
 */
int maxCoinValue(std::vector<int>& coins, int maxCoins, int amount) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目6: 凑零钱的方案（打印所有组合）
 *
 * 给你一个整数数组 coins，表示不同面额的硬币；
 * 以及一个整数 amount，表示总金额。
 *
 * 返回所有可以凑成总金额的硬币组合。
 *
 * 示例:
 *   输入: coins = [1,2,5], amount = 5
 *   输出: [[1,1,1,1,1],[1,1,1,2],[1,2,2],[5]]
 *
 * @param coins 硬币面额数组
 * @param amount 目标金额
 * @return 所有组合
 */
std::vector<std::vector<int>> coinChangeCombinations(std::vector<int>& coins, int amount) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目7: 最少硬币数（有限数量）
 *
 * 给你一个二维数组 coins，其中 coins[i] = [value, count]，
 * 表示面额为 value 的硬币有 count 个。
 * 以及一个整数 amount，表示总金额。
 *
 * 计算并返回可以凑成总金额所需的最少的硬币个数。
 * 如果没有任何一种硬币组合能组成总金额，返回 -1。
 *
 * 示例:
 *   输入: coins = [[1,3],[2,2],[5,1]], amount = 11
 *   输出: 4
 *   解释: 5 + 2 + 2 + 1 + 1 = 11，使用 4 个硬币（有限制）
 *
 * 思路: 多重背包问题
 *
 * @param coins 硬币数组 [面额, 数量]
 * @param amount 目标金额
 * @return 最少硬币数，无解返回 -1
 */
int coinChangeLimited(std::vector<std::vector<int>>& coins, int amount) {
    // TODO: 实现你的解法
    return 0;
}


// ==================== 测试代码 ====================
#include <cassert>

void testCoinChange() {
    std::vector<int> coins = {1, 2, 5};
    assert(coinChange(coins, 11) == 3);

    coins = {2};
    assert(coinChange(coins, 3) == -1);

    coins = {1};
    assert(coinChange(coins, 0) == 0);

    coins = {1, 2, 5};
    assert(coinChange(coins, 100) == 20);

    std::cout << "coinChange: PASSED\n";
}

void testChange() {
    std::vector<int> coins = {1, 2, 5};
    assert(change(5, coins) == 4);

    coins = {2};
    assert(change(3, coins) == 0);

    coins = {10};
    assert(change(10, coins) == 1);

    std::cout << "change: PASSED\n";
}

void testCombinationSum4() {
    std::vector<int> nums = {1, 2, 3};
    assert(combinationSum4(nums, 4) == 7);

    nums = {9};
    assert(combinationSum4(nums, 3) == 0);

    std::cout << "combinationSum4: PASSED\n";
}

void testClimbStairsExtended() {
    std::vector<int> steps = {1, 2};
    assert(climbStairsExtended(5, steps) == 8);

    steps = {1, 2, 5};
    assert(climbStairsExtended(5, steps) == 9);

    std::cout << "climbStairsExtended: PASSED\n";
}

void testMaxCoinValue() {
    std::vector<int> coins = {1, 2, 5};
    assert(maxCoinValue(coins, 3, 11) == 11);

    coins = {3, 7};
    assert(maxCoinValue(coins, 2, 10) == 10);

    std::cout << "maxCoinValue: PASSED\n";
}

void testCoinChangeCombinations() {
    std::vector<int> coins = {1, 2, 5};
    auto result = coinChangeCombinations(coins, 5);
    assert(result.size() == 4);

    std::cout << "coinChangeCombinations: PASSED\n";
}

int main() {
    std::cout << "=== Coin Change Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testCoinChange();
    // testChange();
    // testCombinationSum4();
    // testClimbStairsExtended();
    // testMaxCoinValue();
    // testCoinChangeCombinations();

    std::cout << "All tests passed!\n";
    return 0;
}
