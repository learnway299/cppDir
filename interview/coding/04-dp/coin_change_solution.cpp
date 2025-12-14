/**
 * @file coin_change_solution.cpp
 * @brief 零钱兑换问题 - 参考答案
 */

#include "coin_change.h"
#include <iostream>
#include <cassert>
#include <climits>
#include <algorithm>

namespace CoinChange {

// ==================== 辅助函数 ====================

static void backtrackCoinChange(std::vector<int>& coins, int amount, int start,
                                std::vector<int>& path, std::vector<std::vector<int>>& result) {
    if (amount == 0) {
        result.push_back(path);
        return;
    }
    for (size_t i = start; i < coins.size(); ++i) {
        if (coins[i] <= amount) {
            path.push_back(coins[i]);
            backtrackCoinChange(coins, amount - coins[i], static_cast<int>(i), path, result);
            path.pop_back();
        }
    }
}

// ==================== 参考答案实现 ====================

// 题目1: 零钱兑换（最少硬币数）
int coinChangeSolution(std::vector<int>& coins, int amount) {
    std::vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;
    for (int coin : coins) {
        for (int j = coin; j <= amount; ++j) {
            if (dp[j - coin] != INT_MAX) {
                dp[j] = std::min(dp[j], dp[j - coin] + 1);
            }
        }
    }
    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

// 题目2: 零钱兑换 II（组合数）
int changeSolution(int amount, std::vector<int>& coins) {
    std::vector<int> dp(amount + 1, 0);
    dp[0] = 1;
    for (int coin : coins) {
        for (int j = coin; j <= amount; ++j) {
            dp[j] += dp[j - coin];
        }
    }
    return dp[amount];
}

// 题目3: 组合总和 IV（排列数）
int combinationSum4Solution(std::vector<int>& nums, int target) {
    std::vector<unsigned long long> dp(target + 1, 0);
    dp[0] = 1;
    for (int j = 1; j <= target; ++j) {
        for (int num : nums) {
            if (j >= num) {
                dp[j] += dp[j - num];
            }
        }
    }
    return static_cast<int>(dp[target]);
}

// 题目4: 爬楼梯（扩展版）
int climbStairsExtendedSolution(int n, std::vector<int>& steps) {
    std::vector<int> dp(n + 1, 0);
    dp[0] = 1;
    for (int j = 1; j <= n; ++j) {
        for (int step : steps) {
            if (j >= step) {
                dp[j] += dp[j - step];
            }
        }
    }
    return dp[n];
}

// 题目5: 最大硬币价值
int maxCoinValueSolution(std::vector<int>& coins, int maxCoins, int amount) {
    // dp[i][j] = 使用最多 i 个硬币能凑成金额 j 吗
    std::vector<std::vector<bool>> dp(maxCoins + 1, std::vector<bool>(amount + 1, false));
    dp[0][0] = true;
    for (int coin : coins) {
        for (int i = maxCoins; i >= 1; --i) {
            for (int j = coin; j <= amount; ++j) {
                if (dp[i - 1][j - coin]) {
                    dp[i][j] = true;
                }
            }
        }
    }
    for (int j = amount; j >= 0; --j) {
        for (int i = 0; i <= maxCoins; ++i) {
            if (dp[i][j]) return j;
        }
    }
    return 0;
}

// 题目6: 凑零钱的方案（打印所有组合）
std::vector<std::vector<int>> coinChangeCombinationsSolution(std::vector<int>& coins, int amount) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    backtrackCoinChange(coins, amount, 0, path, result);
    return result;
}

// 题目7: 最少硬币数（有限数量）
int coinChangeLimitedSolution(std::vector<std::vector<int>>& coins, int amount) {
    std::vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;
    for (const auto& coin : coins) {
        int value = coin[0];
        int count = coin[1];
        for (int j = amount; j >= value; --j) {
            for (int k = 1; k <= count && k * value <= j; ++k) {
                if (dp[j - k * value] != INT_MAX) {
                    dp[j] = std::min(dp[j], dp[j - k * value] + k);
                }
            }
        }
    }
    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

// ==================== 测试函数 ====================

void testCoinChangeSolution() {
    std::cout << "=== Coin Change Tests (Solution) ===\n";

    // coinChange
    std::vector<int> coins = {1, 2, 5};
    assert(coinChangeSolution(coins, 11) == 3);
    coins = {2};
    assert(coinChangeSolution(coins, 3) == -1);
    coins = {1};
    assert(coinChangeSolution(coins, 0) == 0);
    std::cout << "  coinChangeSolution: PASSED\n";

    // change
    coins = {1, 2, 5};
    assert(changeSolution(5, coins) == 4);
    coins = {2};
    assert(changeSolution(3, coins) == 0);
    std::cout << "  changeSolution: PASSED\n";

    // combinationSum4
    std::vector<int> nums = {1, 2, 3};
    assert(combinationSum4Solution(nums, 4) == 7);
    nums = {9};
    assert(combinationSum4Solution(nums, 3) == 0);
    std::cout << "  combinationSum4Solution: PASSED\n";

    // climbStairsExtended
    std::vector<int> steps = {1, 2};
    assert(climbStairsExtendedSolution(5, steps) == 8);
    steps = {1, 2, 5};
    assert(climbStairsExtendedSolution(5, steps) == 9);
    std::cout << "  climbStairsExtendedSolution: PASSED\n";

    // coinChangeCombinations
    coins = {1, 2, 5};
    auto result = coinChangeCombinationsSolution(coins, 5);
    assert(result.size() == 4);
    std::cout << "  coinChangeCombinationsSolution: PASSED\n";

    // coinChangeLimited
    std::vector<std::vector<int>> limitedCoins = {{1, 3}, {2, 2}, {5, 1}};
    int res = coinChangeLimitedSolution(limitedCoins, 11);
    assert(res != -1);
    std::cout << "  coinChangeLimitedSolution: PASSED\n";
}

} // namespace CoinChange
