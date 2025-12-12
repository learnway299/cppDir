/**
 * @file coin_change_solution.cpp
 * @brief 零钱兑换问题 - 解答文件
 *
 * 包含 coin_change.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

/**
 * 题目1: 零钱兑换（最少硬币数）
 *
 * 解法: 完全背包（求最小值）
 * dp[j] = 凑成金额 j 所需的最少硬币数
 *
 * 时间复杂度: O(n * amount)
 * 空间复杂度: O(amount)
 */
int coinChange(std::vector<int>& coins, int amount) {
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

/**
 * BFS 解法：层级遍历，找到的第一个解就是最优解
 */
int coinChangeBFS(std::vector<int>& coins, int amount) {
    if (amount == 0) return 0;

    std::vector<bool> visited(amount + 1, false);
    std::vector<int> queue;
    queue.push_back(amount);
    visited[amount] = true;
    int level = 0;

    while (!queue.empty()) {
        ++level;
        std::vector<int> nextLevel;

        for (int curr : queue) {
            for (int coin : coins) {
                int next = curr - coin;
                if (next == 0) return level;
                if (next > 0 && !visited[next]) {
                    visited[next] = true;
                    nextLevel.push_back(next);
                }
            }
        }

        queue = std::move(nextLevel);
    }

    return -1;
}

/**
 * 题目2: 零钱兑换 II（组合数）
 *
 * 解法: 完全背包（组合问题）
 * 组合问题：先遍历物品，再遍历容量
 *
 * 时间复杂度: O(n * amount)
 * 空间复杂度: O(amount)
 */
int change(int amount, std::vector<int>& coins) {
    std::vector<int> dp(amount + 1, 0);
    dp[0] = 1;

    // 先遍历物品（组合问题，避免重复计算不同顺序）
    for (int coin : coins) {
        // 正序遍历（完全背包）
        for (int j = coin; j <= amount; ++j) {
            dp[j] += dp[j - coin];
        }
    }

    return dp[amount];
}

/**
 * 题目3: 组合总和 IV（排列数）
 *
 * 解法: 完全背包变体（排列问题）
 * 排列问题：先遍历容量，再遍历物品
 *
 * 时间复杂度: O(n * target)
 * 空间复杂度: O(target)
 */
int combinationSum4(std::vector<int>& nums, int target) {
    std::vector<unsigned long long> dp(target + 1, 0);
    dp[0] = 1;

    // 先遍历容量（排列问题，考虑不同顺序）
    for (int j = 1; j <= target; ++j) {
        for (int num : nums) {
            if (j >= num) {
                dp[j] += dp[j - num];
            }
        }
    }

    return static_cast<int>(dp[target]);
}

/**
 * 题目4: 爬楼梯（扩展版）
 *
 * 解法: 与组合总和 IV 相同（排列问题）
 * 因为爬楼梯的顺序是有意义的
 */
int climbStairsExtended(int n, std::vector<int>& steps) {
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

/**
 * 题目5: 最大硬币价值
 *
 * 解法: 二维完全背包
 * dp[k][j] = 使用最多 k 个硬币凑成不超过 j 的最大金额
 *
 * 时间复杂度: O(n * maxCoins * amount)
 * 空间复杂度: O(maxCoins * amount)
 */
int maxCoinValue(std::vector<int>& coins, int maxCoins, int amount) {
    // dp[k][j] = 使用恰好 k 个硬币，能否凑成金额 j
    std::vector<std::vector<bool>> dp(maxCoins + 1, std::vector<bool>(amount + 1, false));
    dp[0][0] = true;

    for (int coin : coins) {
        // 逆序遍历硬币数量（限制每种硬币使用次数）
        for (int k = maxCoins; k >= 1; --k) {
            for (int j = coin; j <= amount; ++j) {
                if (dp[k - 1][j - coin]) {
                    dp[k][j] = true;
                }
            }
        }
    }

    // 完全背包版本（每种硬币无限使用）
    std::vector<std::vector<bool>> dp2(maxCoins + 1, std::vector<bool>(amount + 1, false));
    dp2[0][0] = true;

    for (int coin : coins) {
        for (int k = 1; k <= maxCoins; ++k) {
            for (int j = coin; j <= amount; ++j) {
                if (dp2[k - 1][j - coin]) {
                    dp2[k][j] = true;
                }
            }
        }
    }

    // 找最大金额
    int result = 0;
    for (int k = 0; k <= maxCoins; ++k) {
        for (int j = amount; j >= 0; --j) {
            if (dp2[k][j]) {
                result = std::max(result, j);
            }
        }
    }

    return result;
}

/**
 * 简化版本（贪心 + 回溯可能不正确，使用 DP）
 */
int maxCoinValueSimple(std::vector<int>& coins, int maxCoins, int amount) {
    // dp[i] = 使用最多 i 个硬币能凑成的所有金额
    std::vector<std::vector<bool>> reachable(maxCoins + 1, std::vector<bool>(amount + 1, false));
    reachable[0][0] = true;

    for (int k = 1; k <= maxCoins; ++k) {
        reachable[k] = reachable[k - 1];  // 不用第 k 个硬币位
        for (int coin : coins) {
            for (int j = coin; j <= amount; ++j) {
                if (reachable[k - 1][j - coin]) {
                    reachable[k][j] = true;
                }
            }
        }
    }

    for (int j = amount; j >= 0; --j) {
        if (reachable[maxCoins][j]) {
            return j;
        }
    }
    return 0;
}

/**
 * 题目6: 凑零钱的方案（打印所有组合）
 *
 * 解法: 回溯法
 * 时间复杂度: O(n^(amount/minCoin))
 */
void backtrackCoinChange(std::vector<int>& coins, int amount, int start,
                         std::vector<int>& path, std::vector<std::vector<int>>& result) {
    if (amount == 0) {
        result.push_back(path);
        return;
    }

    for (size_t i = start; i < coins.size(); ++i) {
        if (coins[i] <= amount) {
            path.push_back(coins[i]);
            backtrackCoinChange(coins, amount - coins[i], i, path, result);
            path.pop_back();
        }
    }
}

std::vector<std::vector<int>> coinChangeCombinations(std::vector<int>& coins, int amount) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    backtrackCoinChange(coins, amount, 0, path, result);
    return result;
}

/**
 * 题目7: 最少硬币数（有限数量）
 *
 * 解法: 多重背包
 * 可以拆分成 01 背包，或者使用二进制优化
 *
 * 时间复杂度: O(sum(count) * amount) 或 O(n * log(count) * amount)
 */
int coinChangeLimited(std::vector<std::vector<int>>& coins, int amount) {
    std::vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;

    for (const auto& coin : coins) {
        int value = coin[0];
        int count = coin[1];

        // 对每种硬币，最多使用 count 次
        // 逆序遍历金额（类似 01 背包）
        for (int j = amount; j >= value; --j) {
            // 尝试使用 1 到 min(count, j/value) 个这种硬币
            for (int k = 1; k <= count && k * value <= j; ++k) {
                if (dp[j - k * value] != INT_MAX) {
                    dp[j] = std::min(dp[j], dp[j - k * value] + k);
                }
            }
        }
    }

    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

/**
 * 二进制优化版本
 * 将 count 个物品拆分成 1, 2, 4, ..., 2^k, 剩余 的组合
 */
int coinChangeLimitedBinary(std::vector<std::vector<int>>& coins, int amount) {
    std::vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;

    for (const auto& coin : coins) {
        int value = coin[0];
        int count = coin[1];

        // 二进制拆分
        std::vector<std::pair<int, int>> items;  // (价值, 数量)
        for (int k = 1; k <= count; k *= 2) {
            items.push_back({value * k, k});
            count -= k;
        }
        if (count > 0) {
            items.push_back({value * count, count});
        }

        // 01 背包处理每个拆分后的物品
        for (const auto& item : items) {
            int v = item.first;
            int c = item.second;
            for (int j = amount; j >= v; --j) {
                if (dp[j - v] != INT_MAX) {
                    dp[j] = std::min(dp[j], dp[j - v] + c);
                }
            }
        }
    }

    return dp[amount] == INT_MAX ? -1 : dp[amount];
}


// ==================== 背包问题遍历顺序总结 ====================
/**
 * 1. 0-1 背包：逆序遍历容量
 *    for item in items:
 *        for j in range(amount, item-1, -1):
 *            dp[j] = ...
 *
 * 2. 完全背包：正序遍历容量
 *    for item in items:
 *        for j in range(item, amount+1):
 *            dp[j] = ...
 *
 * 3. 组合问题（无序）：先遍历物品，再遍历容量
 *    for item in items:
 *        for j in range(...):
 *
 * 4. 排列问题（有序）：先遍历容量，再遍历物品
 *    for j in range(...):
 *        for item in items:
 */


// ==================== 测试代码 ====================
#include <cassert>

void testCoinChange() {
    std::vector<int> coins = {1, 2, 5};
    assert(coinChange(coins, 11) == 3);
    assert(coinChangeBFS(coins, 11) == 3);

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
    assert(maxCoinValueSimple(coins, 3, 11) == 11);

    coins = {3, 7};
    assert(maxCoinValueSimple(coins, 2, 10) == 10);

    std::cout << "maxCoinValue: PASSED\n";
}

void testCoinChangeCombinations() {
    std::vector<int> coins = {1, 2, 5};
    auto result = coinChangeCombinations(coins, 5);
    assert(result.size() == 4);

    coins = {2};
    result = coinChangeCombinations(coins, 3);
    assert(result.empty());

    std::cout << "coinChangeCombinations: PASSED\n";
}

void testCoinChangeLimited() {
    std::vector<std::vector<int>> coins = {{1, 3}, {2, 2}, {5, 1}};
    int result = coinChangeLimited(coins, 11);
    // 5 + 2 + 2 + 1 + 1 = 11, 使用 5 个硬币
    // 或者其他组合
    assert(result != -1);

    assert(coinChangeLimitedBinary(coins, 11) == result);

    std::cout << "coinChangeLimited: PASSED\n";
}

int main() {
    std::cout << "=== Coin Change Solution Tests ===\n";

    testCoinChange();
    testChange();
    testCombinationSum4();
    testClimbStairsExtended();
    testMaxCoinValue();
    testCoinChangeCombinations();
    testCoinChangeLimited();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
