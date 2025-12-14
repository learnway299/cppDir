/**
 * @file knapsack_solution.cpp
 * @brief 背包问题 - 参考答案
 */

#include "knapsack.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <climits>

namespace Knapsack {

// ==================== 参考答案实现 ====================

// 题目1: 0-1 背包问题
int knapsack01Solution(int W, std::vector<int>& weight, std::vector<int>& value) {
    int n = static_cast<int>(weight.size());
    std::vector<int> dp(W + 1, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = W; j >= weight[i]; --j) {
            dp[j] = std::max(dp[j], dp[j - weight[i]] + value[i]);
        }
    }
    return dp[W];
}

// 题目2: 分割等和子集
bool canPartitionSolution(std::vector<int>& nums) {
    int sum = std::accumulate(nums.begin(), nums.end(), 0);
    if (sum % 2 != 0) return false;
    int target = sum / 2;
    std::vector<bool> dp(target + 1, false);
    dp[0] = true;
    for (int num : nums) {
        for (int j = target; j >= num; --j) {
            dp[j] = dp[j] || dp[j - num];
        }
    }
    return dp[target];
}

// 题目3: 目标和
int findTargetSumWaysSolution(std::vector<int>& nums, int target) {
    int sum = std::accumulate(nums.begin(), nums.end(), 0);
    if ((sum + target) % 2 != 0 || sum + target < 0) return 0;
    if (std::abs(target) > sum) return 0;
    int P = (sum + target) / 2;
    std::vector<int> dp(P + 1, 0);
    dp[0] = 1;
    for (int num : nums) {
        for (int j = P; j >= num; --j) {
            dp[j] += dp[j - num];
        }
    }
    return dp[P];
}

// 题目4: 最后一块石头的重量 II
int lastStoneWeightIISolution(std::vector<int>& stones) {
    int sum = std::accumulate(stones.begin(), stones.end(), 0);
    int target = sum / 2;
    std::vector<int> dp(target + 1, 0);
    for (int stone : stones) {
        for (int j = target; j >= stone; --j) {
            dp[j] = std::max(dp[j], dp[j - stone] + stone);
        }
    }
    return sum - 2 * dp[target];
}

// 题目5: 完全背包 - 零钱兑换 II（组合数）
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

// 题目6: 完全背包 - 组合总和 IV（排列数）
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

// 题目7: 完全平方数
int numSquaresSolution(int n) {
    std::vector<int> dp(n + 1, INT_MAX);
    dp[0] = 0;
    for (int i = 1; i * i <= n; ++i) {
        int square = i * i;
        for (int j = square; j <= n; ++j) {
            if (dp[j - square] != INT_MAX) {
                dp[j] = std::min(dp[j], dp[j - square] + 1);
            }
        }
    }
    return dp[n];
}

// 题目8: 单词拆分
bool wordBreakSolution(std::string s, std::vector<std::string>& wordDict) {
    std::unordered_set<std::string> wordSet(wordDict.begin(), wordDict.end());
    int n = static_cast<int>(s.size());
    std::vector<bool> dp(n + 1, false);
    dp[0] = true;
    for (int i = 1; i <= n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (dp[j] && wordSet.count(s.substr(j, i - j))) {
                dp[i] = true;
                break;
            }
        }
    }
    return dp[n];
}

// 题目9: 一和零
int findMaxFormSolution(std::vector<std::string>& strs, int m, int n) {
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    for (const std::string& str : strs) {
        int zeros = static_cast<int>(std::count(str.begin(), str.end(), '0'));
        int ones = static_cast<int>(str.size()) - zeros;
        for (int i = m; i >= zeros; --i) {
            for (int j = n; j >= ones; --j) {
                dp[i][j] = std::max(dp[i][j], dp[i - zeros][j - ones] + 1);
            }
        }
    }
    return dp[m][n];
}

// ==================== 测试函数 ====================

void testKnapsackSolution() {
    std::cout << "=== Knapsack Tests (Solution) ===\n";

    // knapsack01
    std::vector<int> weight = {1, 3, 4};
    std::vector<int> value = {15, 20, 30};
    assert(knapsack01Solution(4, weight, value) == 35);
    weight = {1, 2, 3};
    value = {6, 10, 12};
    assert(knapsack01Solution(5, weight, value) == 22);
    std::cout << "  knapsack01Solution: PASSED\n";

    // canPartition
    std::vector<int> nums = {1, 5, 11, 5};
    assert(canPartitionSolution(nums) == true);
    nums = {1, 2, 3, 5};
    assert(canPartitionSolution(nums) == false);
    std::cout << "  canPartitionSolution: PASSED\n";

    // findTargetSumWays
    nums = {1, 1, 1, 1, 1};
    assert(findTargetSumWaysSolution(nums, 3) == 5);
    nums = {1};
    assert(findTargetSumWaysSolution(nums, 1) == 1);
    std::cout << "  findTargetSumWaysSolution: PASSED\n";

    // lastStoneWeightII
    std::vector<int> stones = {2, 7, 4, 1, 8, 1};
    assert(lastStoneWeightIISolution(stones) == 1);
    std::cout << "  lastStoneWeightIISolution: PASSED\n";

    // numSquares
    assert(numSquaresSolution(12) == 3);
    assert(numSquaresSolution(13) == 2);
    assert(numSquaresSolution(1) == 1);
    std::cout << "  numSquaresSolution: PASSED\n";

    // wordBreak
    std::vector<std::string> dict = {"leet", "code"};
    assert(wordBreakSolution("leetcode", dict) == true);
    dict = {"apple", "pen"};
    assert(wordBreakSolution("applepenapple", dict) == true);
    std::cout << "  wordBreakSolution: PASSED\n";

    // findMaxForm
    std::vector<std::string> strs = {"10", "0001", "111001", "1", "0"};
    assert(findMaxFormSolution(strs, 5, 3) == 4);
    std::cout << "  findMaxFormSolution: PASSED\n";
}

} // namespace Knapsack
